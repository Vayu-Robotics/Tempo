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

struct FRenderRequest
{
	FRenderRequest(const FIntPoint& ImageSizeIn, int32 FrameCounterIn)
		: ImageSize(ImageSizeIn), FrameCounter(FrameCounterIn), Image(TArray(&FColor::Green, ImageSize.X * ImageSize.Y)) {}
	
	FIntPoint ImageSize = FIntPoint::ZeroValue;
	int32 FrameCounter;
	TArray<FColor> Image;
	FRenderCommandFence RenderFence;
};

struct FImageRequest
{
	FImageRequest(int32 QualityIn, const TResponseDelegate<TempoSensors::Image>& ResponseContinuationIn)
		: Quality(QualityIn), ResponseContinuation(ResponseContinuationIn) {}
	
	int32 Quality = 100;
	TResponseDelegate<TempoSensors::Image> ResponseContinuation;
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
	
	TMap<int32, FImageRequest> PendingImageRequests;
	
	TMap<int32, TArray<TUniquePtr<FRenderRequest>>> PendingRenderRequests;
};
