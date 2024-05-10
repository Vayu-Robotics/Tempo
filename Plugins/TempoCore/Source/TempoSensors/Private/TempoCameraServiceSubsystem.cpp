// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoCameraServiceSubsystem.h"

#include "TempoCamera.h"

#include "TempoSensors/Camera.grpc.pb.h"

using CameraService = TempoSensors::CameraService::AsyncService;
using CameraDescriptor = TempoSensors::CameraDescriptor;
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
	AvailableCamerasResponse Response;
	for (TObjectIterator<UTempoCamera> It; It; ++It)
	{
		const UTempoCamera* Camera = *It;
		Response.add_available_cameras()->set_id(Camera->GetId());
	}
	ResponseContinuation.ExecuteIfBound(Response, grpc::Status_OK);
}

void UTempoCameraServiceSubsystem::StreamImages(const TempoSensors::StreamImagesRequest& Request, const TResponseDelegate<TempoSensors::Image>& ResponseContinuation)
{
	PendingImageResponseContinuations.Add(Request.camera_id(), ResponseContinuation);
}

void UTempoCameraServiceSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TMap<int32, TArray<FColor>> OutImageDatas;
	for (const auto& PendingImage : PendingImages)
	{
		const int32 CameraId = PendingImage.Key;
		FTextureRenderTargetResource* TextureResource = PendingImage.Value;
		if (TResponseDelegate<Image>* ResponseContinuation = PendingImageResponseContinuations.Find(CameraId))
		{
			OutImageDatas.Add(CameraId, TArray(&FColor::Green, TextureResource->GetSizeX() * TextureResource->GetSizeY()));
			EnqueueTextureRead(TextureResource, OutImageDatas[CameraId]);
		}
	}
	
	if (OutImageDatas.Num() > 0)
	{
		FlushRenderingCommands();
	}

	for (const auto& OutImageData : OutImageDatas)
	{
		const int32 CameraId = OutImageData.Key;
		const TArray<FColor>& Pixels = OutImageData.Value;
		TempoSensors::Image Image;
		Image.set_width(PendingImages[CameraId]->GetSizeX());
		Image.set_height(PendingImages[CameraId]->GetSizeY());
		Image.mutable_pixels()->Reserve(Pixels.Num());
		for (const FColor& Pixel : Pixels)
		{
			Image.add_pixels(Pixel.ToPackedARGB());
		}
		PendingImageResponseContinuations[CameraId].ExecuteIfBound(Image, grpc::Status_OK);
		PendingImageResponseContinuations.Remove(CameraId);
	}

	PendingImages.Empty();
}

TStatId UTempoCameraServiceSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTempoCameraServiceSubsystem, STATGROUP_Tickables);
}

void UTempoCameraServiceSubsystem::EnqueueTextureRead(FTextureRenderTargetResource* TextureResource, TArray<FColor>& OutImageData)
{
	const FIntRect Rect = FIntRect(0, 0, TextureResource->GetSizeX(), TextureResource->GetSizeY());
	const FReadSurfaceDataFlags Flags = FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX);

	// Read the render target surface data back.	
	struct FReadSurfaceContext
	{
		FRenderTarget* SrcRenderTarget;
		TArray<FColor>* OutData;
		FIntRect Rect;
		FReadSurfaceDataFlags Flags;
	};
	
	OutImageData.Reset();
	FReadSurfaceContext Context =
	{
		TextureResource,
		&OutImageData,
		Rect,
		Flags
	};

	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
		[Context](FRHICommandListImmediate& RHICmdList)
		{
			RHICmdList.ReadSurfaceData(
				Context.SrcRenderTarget->GetRenderTargetTexture(),
				Context.Rect,
				*Context.OutData,
				Context.Flags
				);
		});
}

void UTempoCameraServiceSubsystem::AddCameraFrame(int32 CameraId, FTextureRenderTargetResource* TextureResource)
{
	PendingImages.Add(CameraId, TextureResource);
}
