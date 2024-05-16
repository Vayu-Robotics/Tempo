// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoScriptable.h"

#include "TempoScriptingServer.h"

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RHIGPUReadback.h"

#include "TempoCameraServiceSubsystem.generated.h"

namespace TempoScripting
{
	class Empty;
}

namespace TempoSensors
{
	class AvailableCamerasRequest;
	class AvailableCamerasResponse;
	class StreamImagesRequest;
	class Image;
}

struct FTextureRead
{
	FTextureRead(const FIntPoint& ImageSizeIn, int32 FrameCounterIn)
		: ImageSize(ImageSizeIn), FrameCounter(FrameCounterIn), Image(TArray(&FColor::Green, ImageSize.X * ImageSize.Y)) {}
	
	FIntPoint ImageSize;
	int32 FrameCounter;
	TArray<FColor> Image;
	FRenderCommandFence RenderFence;
};

struct FImageRequest
{
	FImageRequest(int32 QualityIn, const TResponseDelegate<TempoSensors::Image>& ResponseContinuationIn)
		: Quality(QualityIn), ResponseContinuation(ResponseContinuationIn) {}
	
	int32 Quality;
	TResponseDelegate<TempoSensors::Image> ResponseContinuation;
};

struct FRequestedCamera
{
	TArray<FImageRequest> PendingImageRequests;
	
	int32 GetNumPendingTextureReads() const { return PendingTextureReads.Num(); }
	
	bool HasOutstandingTextureReads() const { return !PendingTextureReads.IsEmpty() && !PendingTextureReads[0]->RenderFence.IsFenceComplete(); }
	
	void EnqueuePendingTextureRead(FTextureRead* TextureRead)
	{
		PendingTextureReads.Emplace(TextureRead);
	}

	TUniquePtr<FTextureRead> DequeuePendingTextureRead()
	{
		if (!PendingTextureReads.IsEmpty() && PendingTextureReads[0]->RenderFence.IsFenceComplete())
		{
			TUniquePtr<FTextureRead> TextureRead(PendingTextureReads[0].Release());
			PendingTextureReads.RemoveAt(0);
			return MoveTemp(TextureRead);
		}
		return nullptr;
	}

private:
	// A "Queue" of pending texture reads.
	TArray<TUniquePtr<FTextureRead>> PendingTextureReads;
};

UCLASS()
class TEMPOSENSORS_API UTempoCameraServiceSubsystem : public UTickableWorldSubsystem, public ITempoWorldScriptable
{
	GENERATED_BODY()
	
public:
	virtual void RegisterWorldServices(UTempoScriptingServer* ScriptingServer) override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	bool HasPendingRequestForCamera(int32 CameraId) const;

	void SendImage(int32 CameraId, int32 FrameCounter, FTextureRenderTargetResource* TextureResource);

private:
	void GetAvailableCameras(const TempoSensors::AvailableCamerasRequest& Request, const TResponseDelegate<TempoSensors::AvailableCamerasResponse>& ResponseContinuation) const;

	void StreamImages(const TempoSensors::StreamImagesRequest& Request, const TResponseDelegate<TempoSensors::Image>& ResponseContinuation);
	
	TMap<int32, FRequestedCamera> RequestedCameras;
};
