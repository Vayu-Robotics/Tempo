// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoScriptable.h"

#include "TempoScriptingServer.h"

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

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

UCLASS()
class TEMPOSENSORS_API UTempoCameraServiceSubsystem : public UTickableWorldSubsystem, public ITempoWorldScriptable
{
	GENERATED_BODY()
	
public:
	virtual void RegisterWorldServices(UTempoScriptingServer* ScriptingServer) override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	void AddCameraFrame(int32 CameraId, FTextureRenderTargetResource* TextureResource);

private:
	void GetAvailableCameras(const TempoSensors::AvailableCamerasRequest& Request, const TResponseDelegate<TempoSensors::AvailableCamerasResponse>& ResponseContinuation) const;

	void StreamImages(const TempoSensors::StreamImagesRequest& Request, const TResponseDelegate<TempoSensors::Image>& ResponseContinuation);

	static void EnqueueTextureRead(FTextureRenderTargetResource* TextureResource, TArray<FColor>& OutImageData);

	TMap<int32, TResponseDelegate<TempoSensors::Image>> PendingImageResponseContinuations;

	TMap<int32, FTextureRenderTargetResource*> PendingImages;
};
