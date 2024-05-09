// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoCameraServiceSubsystem.h"

#include "TempoCamera.h"

#include "TempoSensors/Camera.grpc.pb.h"

using CameraService = TempoSensors::CameraService::AsyncService;
using AvailableCamerasRequest = TempoSensors::AvailableCamerasRequest;
using AvailableCamerasResponse = TempoSensors::AvailableCamerasResponse;
using StreamImagesRequest = TempoSensors::StreamImagesRequest;
using Image = TempoSensors::Image;

void UTempoCameraServiceSubsystem::RegisterWorldServices(UTempoScriptingServer* ScriptingServer)
{
	ScriptingServer->RegisterService<CameraService>(
		TSimpleRequestHandler<CameraService, AvailableCamerasRequest, AvailableCamerasResponse>(&CameraService::RequestGetAvailableCameras).BindUObject(this, &UTempoCameraServiceSubsystem::GetAvailableCameras),
		TStreamingRequestHandler<CameraService, StreamImagesRequest, Image>(&CameraService::RequestStreamImages).BindUObject(this, &UTempoCameraServiceSubsystem::StreamImages)
		);
}

void UTempoCameraServiceSubsystem::GetAvailableCameras(const TempoSensors::AvailableCamerasRequest& Request, const TResponseDelegate<TempoSensors::AvailableCamerasResponse>& ResponseContinuation) const
{
	
}

void UTempoCameraServiceSubsystem::StreamImages(const TempoSensors::StreamImagesRequest& Request, const TResponseDelegate<TempoSensors::Image>& ResponseContinuation)
{
	for (TObjectIterator<UTempoCamera> It; It; ++It)
	{
		UTempoCamera* Camera = *It;
		if (Camera->GetId() == Request.camera_id())
		{
			Camera->EnqueueImageStreamContinuation(ResponseContinuation);
		}
	}
}
