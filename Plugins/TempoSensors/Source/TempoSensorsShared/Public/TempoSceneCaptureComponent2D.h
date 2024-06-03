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

	void FlushPendingTextureReads() const
	{
		for (const auto& PendingTextureRead : PendingTextureReads)
		{
			PendingTextureRead->RenderFence.Wait();
		}
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

	virtual TOptional<TFuture<void>> FlushMeasurementResponses() override { return TOptional<TFuture<void>>(); }

	virtual bool HasPendingRenderingCommands() override { return false; }

	virtual void FlushPendingRenderingCommands() const override {}
	
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
	};

	FReadSurfaceContext Context = {
		TextureTarget->GameThread_GetRenderTargetResource(),
		&TextureRead->Image
	};

	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
		[Context](FRHICommandListImmediate& RHICmdList)
		{
			void* OutBuffer;
			int32 SurfaceWidth, SurfaceHeight;
			const FIntPoint TextureSize = Context.RenderTarget->GetSizeXY();
			RHICmdList.MapStagingSurface(Context.RenderTarget->GetRenderTargetTexture(), OutBuffer, SurfaceWidth, SurfaceHeight);
			FMemory::Memcpy(Context.Image->GetData(), OutBuffer, SurfaceWidth * SurfaceHeight * sizeof(PixelType));
			RHICmdList.UnmapStagingSurface(Context.RenderTarget->GetRenderTargetTexture());
	});
	
	TextureRead->RenderFence.BeginFence();

	return TextureRead;
}
