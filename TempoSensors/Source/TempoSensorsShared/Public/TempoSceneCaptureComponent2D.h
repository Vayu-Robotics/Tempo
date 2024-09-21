// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoSensorInterface.h"

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

#include "TempoSceneCaptureComponent2D.generated.h"

struct FTextureRead
{
	FTextureRead(const FIntPoint& ImageSizeIn, int32 SequenceIdIn, double CaptureTimeIn, const FString& OwnerNameIn, const FString& SensorNameIn)
		: ImageSize(ImageSizeIn), SequenceId(SequenceIdIn), CaptureTime(CaptureTimeIn), OwnerName(OwnerNameIn), SensorName(SensorNameIn) {}

	virtual ~FTextureRead() {}

	virtual void BeginRead(const FRenderTarget* RenderTarget, const FTextureRHIRef& TextureRHICopy) = 0;

	virtual FName GetType() const = 0;

	void WaitForReadCompleted() const
	{
		while (!bReadCompleted)
		{
			FPlatformProcess::Sleep(1e-4);
		}
	}
	
	FIntPoint ImageSize;
	int32 SequenceId;
	double CaptureTime;
	const FString OwnerName;
	const FString SensorName;
	TAtomic<bool> bReadCompleted = false;
};

template <typename PixelType>
struct TTextureReadBase : FTextureRead
{
	TTextureReadBase(const FIntPoint& ImageSizeIn, int32 SequenceIdIn, double CaptureTimeIn, const FString& OwnerNameIn, const FString& SensorNameIn)
		   : FTextureRead(ImageSizeIn, SequenceIdIn, CaptureTimeIn, OwnerNameIn, SensorNameIn)
	{
		Image.SetNumUninitialized(ImageSize.X * ImageSize.Y);
	}

	virtual void BeginRead(const FRenderTarget* RenderTarget, const FTextureRHIRef& TextureRHICopy) override
	{
		UE_LOG(LogTemp, Warning, TEXT("Beginning texture read %s"), *GetType().ToString());

		FRHICommandListImmediate& RHICmdList = FRHICommandListImmediate::Get();

		// Then, transition our TextureTarget to be copyable.
		RHICmdList.Transition(FRHITransitionInfo(RenderTarget->GetRenderTargetTexture(), ERHIAccess::Unknown, ERHIAccess::CopySrc));

		// Then, copy our TextureTarget to another that can be mapped and read by the CPU.
		RHICmdList.CopyTexture(RenderTarget->GetRenderTargetTexture(), TextureRHICopy, FRHICopyTextureInfo());

		// Write a GPU fence to wait for the above copy to complete before reading the data.
		const FGPUFenceRHIRef Fence = RHICreateGPUFence(TEXT("TempoCameraTextureRead"));
		RHICmdList.WriteGPUFence(Fence);

		// Lastly, read the raw data from the copied TextureTarget on the CPU.
		void* OutBuffer;
		int32 SurfaceWidth, SurfaceHeight;
		GDynamicRHI->RHIMapStagingSurface(TextureRHICopy, Fence, OutBuffer, SurfaceWidth, SurfaceHeight, RHICmdList.GetGPUMask().ToIndex());
		FMemory::Memcpy(Image.GetData(), OutBuffer, SurfaceWidth * SurfaceHeight * sizeof(PixelType));
		RHICmdList.UnmapStagingSurface(TextureRHICopy);
		
		bReadCompleted = true;
	}
	
	TArray<PixelType> Image;
};

template <typename PixelType>
struct TTextureRead : TTextureReadBase<PixelType>
{
	using TTextureReadBase<PixelType>::TTextureReadBase;
};

struct FTextureReadQueue
{
	int32 GetNumPendingTextureReads() const
	{
		FScopeLock Lock(&Mutex);
		return PendingTextureReads.Num();
	}
	
	bool HasOutstandingTextureReads() const
	{
		FScopeLock Lock(&Mutex);
		return !PendingTextureReads.IsEmpty() && !PendingTextureReads[0]->bReadCompleted;
	}
	
	void EnqueuePendingTextureRead(FTextureRead* TextureRead)
	{
		FScopeLock Lock(&Mutex);
		PendingTextureReads.Emplace(TextureRead);
	}

	void Empty()
	{
		FScopeLock Lock(&Mutex);
		PendingTextureReads.Empty();
	}

	void BeginNextPendingTextureRead(const FRenderTarget* RenderTarget, const FTextureRHIRef& TextureRHICopy)
	{
		FScopeLock Lock(&Mutex);
		if (!PendingTextureReads.IsEmpty())
		{
			PendingTextureReads[0]->BeginRead(RenderTarget, TextureRHICopy);
		}
	}

	void SkipNextPendingTextureRead()
	{
		FScopeLock Lock(&Mutex);
		if (!PendingTextureReads.IsEmpty())
		{
			PendingTextureReads.RemoveAt(0);
		}
	}

	void FlushPendingTextureReads() const
	{
		// Is this safe?
		// FScopeLock Lock(&Mutex);
		for (const auto& PendingTextureRead : PendingTextureReads)
		{
			PendingTextureRead->WaitForReadCompleted();
		}
	}

	TUniquePtr<FTextureRead> DequeuePendingTextureRead()
	{
		FScopeLock Lock(&Mutex);
		if (!PendingTextureReads.IsEmpty() && PendingTextureReads[0]->bReadCompleted)
		{
			TUniquePtr<FTextureRead> TextureRead(PendingTextureReads[0].Release());
			PendingTextureReads.RemoveAt(0);
			return MoveTemp(TextureRead);
		}
		return nullptr;
	}

private:
	TArray<TUniquePtr<FTextureRead>> PendingTextureReads;
	mutable FCriticalSection Mutex;
};

UCLASS(Abstract)
class TEMPOSENSORSSHARED_API UTempoSceneCaptureComponent2D : public USceneCaptureComponent2D, public ITempoSensorInterface
{
	GENERATED_BODY()

public:
	UTempoSceneCaptureComponent2D();

	virtual void BeginPlay() override;

	virtual void UpdateSceneCaptureContents(FSceneInterface* Scene) override;

	virtual FString GetOwnerName() const override;

	virtual FString GetSensorName() const override;

	virtual float GetRate() const override { return RateHz; }
	
	virtual const TArray<TEnumAsByte<EMeasurementType>>& GetMeasurementTypes() const override { return MeasurementTypes; }

	virtual void OnFrameRenderCompleted(bool bBlock) override;
	
	virtual bool HasPendingRenderingCommands() override { return TextureReadQueue.HasOutstandingTextureReads(); }
	
	virtual void FlushPendingRenderingCommands() const override;

	virtual TOptional<TFuture<void>> FlushMeasurementResponses() override;

protected:
	virtual bool HasPendingRequests() const { return false; }

	virtual FTextureRead* MakeTextureRead() const PURE_VIRTUAL(UTempoSceneCaptureComponent2D::MakeTextureRead, return nullptr; );

	virtual TFuture<void> DecodeAndRespond(TUniquePtr<FTextureRead> TextureRead) PURE_VIRTUAL(UTempoSceneCaptureComponent2D::DecodeAndRespond, return TFuture<void>(); );

	virtual int32 GetMaxTextureQueueSize() const PURE_VIRTUAL(UTempoSceneCaptureComponent2D::GetMaxTextureQueueSize, return 0; );

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

	FTextureReadQueue TextureReadQueue;

	FGPUFenceRHIRef RenderFence;

	FRenderCommandFence TextureInitFence;

	// We must copy our TextureTarget's resource here before reading it on the CPU.
	FTextureRHIRef TextureRHICopy;

	FTimerHandle TimerHandle;
};
