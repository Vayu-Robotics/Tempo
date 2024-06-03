// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoSensorInterface.h"

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

#include "TempoSceneCaptureComponent2D.generated.h"

template <typename PixelType>
struct TTextureRead
{
	TTextureRead(const FIntPoint& ImageSizeIn, int32 SequenceIdIn, double CaptureTimeIn)
		   : ImageSize(ImageSizeIn), SequenceId(SequenceIdIn), CaptureTime(CaptureTimeIn)
	{
		Image.SetNumUninitialized(ImageSize.X * ImageSize.Y);
	}

	FIntPoint ImageSize;
	int32 SequenceId;
	double CaptureTime;
	TArray<PixelType> Image;
	FRenderCommandFence RenderFence;
};

template <typename PixelType>
struct TTextureReadQueue
{
	int32 GetNumPendingTextureReads() const { return PendingTextureReads.Num(); }
	
	bool HasOutstandingTextureReads() const { return !PendingTextureReads.IsEmpty() && !PendingTextureReads[0]->RenderFence.IsFenceComplete(); }
	
	void EnqueuePendingTextureRead(TTextureRead<PixelType>* TextureRead)
	{
		PendingTextureReads.Emplace(TextureRead);
	}

	TUniquePtr<TTextureRead<PixelType>> DequeuePendingTextureRead()
	{
		if (!PendingTextureReads.IsEmpty() && PendingTextureReads[0]->RenderFence.IsFenceComplete())
		{
			TUniquePtr<TTextureRead<PixelType>> TextureRead(PendingTextureReads[0].Release());
			PendingTextureReads.RemoveAt(0);
			return MoveTemp(TextureRead);
		}
		return nullptr;
	}

private:
	TArray<TUniquePtr<TTextureRead<PixelType>>> PendingTextureReads;
};

UCLASS(Abstract)
class TEMPOSENSORSSHARED_API UTempoSceneCaptureComponent2D : public USceneCaptureComponent2D, public ITempoSensorInterface
{
	GENERATED_BODY()

public:
	UTempoSceneCaptureComponent2D();

	virtual FString GetOwnerName() const override;

	virtual FString GetSensorName() const override;

	virtual float GetRate() const override { return RateHz; }
	
	virtual const TArray<TEnumAsByte<EMeasurementType>>& GetMeasurementTypes() const override { return MeasurementTypes; }
	
	virtual void UpdateSceneCaptureContents(FSceneInterface* Scene) override;

	virtual void FlushMeasurementResponses() override {}

	virtual bool HasPendingRenderingCommands() override { return false; }
	
protected:
	virtual void BeginPlay() override;

	virtual bool HasPendingRequests() const { return false; }

	template <typename PixelType>
	TTextureRead<PixelType>* EnqueueTextureRead() const;

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ETextureRenderTargetFormat> RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<EPixelFormat> PixelFormatOverride = EPixelFormat::PF_Unknown;
	
	UPROPERTY(EditAnywhere)
	float RateHz = 10.0;

	UPROPERTY(VisibleAnywhere)
	TArray<TEnumAsByte<EMeasurementType>> MeasurementTypes;

	UPROPERTY(EditAnywhere)
	FIntPoint SizeXY = FIntPoint(960, 540);

	UPROPERTY(VisibleAnywhere)
	int32 SequenceId = 0;
	
	void InitRenderTarget();
	
private:
	void MaybeCapture();
	
	FTimerHandle TimerHandle;

#if PLATFORM_LINUX
	// We must copy our TextureTarget's resource here before reading it on the CPU, due to a Vulkan limitation.
	mutable FTextureRHIRef TextureRHICopy;
#endif
};

// Enqueue a read of our full render target, reinterpreting the raw pixels as PixelType.
// The render target pixel format must already be a compatible type.
template <typename PixelType>
TTextureRead<PixelType>* UTempoSceneCaptureComponent2D::EnqueueTextureRead() const
{
	TTextureRead<PixelType>* TextureRead = new TTextureRead<PixelType>(SizeXY, SequenceId, GetWorld()->GetTimeSeconds());
	
	struct FReadSurfaceContext{
		FRenderTarget* RenderTarget;
		TArray<PixelType>* Image;
#if PLATFORM_LINUX
        FTextureRHIRef TextureRHICopy;
#endif
	};

	FReadSurfaceContext Context = {
		TextureTarget->GameThread_GetRenderTargetResource(),
		&TextureRead->Image
#if PLATFORM_LINUX
        , TextureRHICopy
#endif
	};

	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
		[Context](FRHICommandListImmediate& RHICmdList)
		{
			void* OutBuffer;
			int32 SurfaceWidth, SurfaceHeight;
			const FIntPoint TextureSize = Context.RenderTarget->GetSizeXY();
#if PLATFORM_LINUX
			// Vulkan checks that a texture has TexCreate_CPUReadback flag set in MapStagingSurface, which our TextureTarget does not.
			// However we can work around this by creating a texture with the flag set and copying our TextureTarget there first.
			// Credit for this idea to https://forums.unrealengine.com/t/lock-read-rhitexture-on-vulkan/465529
			RHICmdList.CopyTexture(Context.RenderTarget->GetRenderTargetTexture(), Context.TextureRHICopy, FRHICopyTextureInfo());
			FRHITexture* RHITexture = Context.TextureRHICopy;
#else
			FRHITexture* RHITexture = Context.RenderTarget->GetRenderTargetTexture();
#endif
			RHICmdList.MapStagingSurface(RHITexture, OutBuffer, SurfaceWidth, SurfaceHeight);
			for (int32 Y = 0; Y < SurfaceHeight; ++Y)
			{
				for (int32 X = 0; X < TextureSize.X; ++X)
				{
					const int32 PixelOffset = X + (Y * SurfaceWidth);
					(*Context.Image)[X + Y * TextureSize.X] = *(static_cast<PixelType*>(OutBuffer) + PixelOffset);
				}
			}
			RHICmdList.UnmapStagingSurface(Context.RenderTarget->GetRenderTargetTexture());
	});
	
	TextureRead->RenderFence.BeginFence();

	return TextureRead;
}
