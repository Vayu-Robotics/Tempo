// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoCameraServiceSubsystem.h"

#include "ImageUtils.h"
#include "TempoCamera.h"
#include "TempoSensors.h"

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

int32 QualityFromCompressionLevel(TempoSensors::CompressionLevel CompressionLevel)
{
	// Inspired by FJpegImageWrapper::Compress()
	switch (CompressionLevel)
	{
	case TempoSensors::LOSSLESS:
		{
			return 100;
		}
	case TempoSensors::VERY_LOW:
		{
			return 90;
		}
	case TempoSensors::LOW:
		{
			return 80;
		}
	case TempoSensors::MID:
		{
			return 70;
		}
	case TempoSensors::HIGH:
		{
			return 60;
		}
	case TempoSensors::VERY_HIGH:
		{
			return 50;
		}
	case TempoSensors::MAX:
		{
			return 40;
		}
	default:
		{
			checkf(false, TEXT("Unhandled compression level"));
			return 0;
		}
	}
}

void UTempoCameraServiceSubsystem::StreamImages(const TempoSensors::StreamImagesRequest& Request, const TResponseDelegate<TempoSensors::Image>& ResponseContinuation)
{
	const int32 Quality = QualityFromCompressionLevel(Request.compression_level());
	PendingImageRequests.Add(Request.camera_id(), FImageRequest(Quality, ResponseContinuation));
}

void UTempoCameraServiceSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If we're waiting on any render requests, flush all rendering commands.
	for (const auto& PendingImageRequest: PendingImageRequests)
	{
		const FImageRequest& ImageRequest = PendingImageRequest.Value;
		if (ImageRequest.RenderRequest.IsValid())
		{
			FlushRenderingCommands();
			break;
		}
	}

	for (auto PendingImageRequestIt = PendingImageRequests.CreateIterator(); PendingImageRequestIt; ++PendingImageRequestIt)
	{
		const int32 CameraId = PendingImageRequestIt->Key;
		FImageRequest& ImageRequest = PendingImageRequestIt->Value;
		if (ImageRequest.RenderRequest.IsValid() && ImageRequest.RenderRequest->RenderFence.IsFenceComplete())
		{
			// Compress
			TArray64<uint8> CompressedData;
			FImageUtils::CompressImage(CompressedData,
						TEXT("jpg"),
						FImageView(ImageRequest.RenderRequest->Image.GetData(), ImageRequest.RenderRequest->ImageSize.X, ImageRequest.RenderRequest->ImageSize.Y),
						ImageRequest.Quality);

			// Pack into message
			TempoSensors::Image Image;	
			Image.set_width(ImageRequest.RenderRequest->ImageSize.X);
			Image.set_height(ImageRequest.RenderRequest->ImageSize.Y);
			Image.set_data(CompressedData.GetData(), CompressedData.Num());

			// Send
			ImageRequest.ResponseContinuation.ExecuteIfBound(Image, grpc::Status_OK);

			PendingImageRequests.Remove(CameraId);
		}
	}
}

TStatId UTempoCameraServiceSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTempoCameraServiceSubsystem, STATGROUP_Tickables);
}

void UTempoCameraServiceSubsystem::MaybeSendImage(int32 CameraId, FTextureRenderTargetResource* TextureResource)
{
	FImageRequest* ImageRequest = PendingImageRequests.Find(CameraId);
	if (!ImageRequest)
	{
		// No one's asking for this camera's images - don't bother rendering it.
		return;
	}

	if (ImageRequest->RenderRequest.IsValid())
	{
		UE_LOG(LogTempoSensors, Warning, TEXT("Fell behind while rendering images from camera %d. Dropping image."), CameraId);
		return;
	}

	ImageRequest->RenderRequest = MakeUnique<FRenderRequest>(TextureResource->GetSizeXY());
	
	struct FReadSurfaceContext{
		FRenderTarget* SrcRenderTarget;
		TArray<FColor>* OutData;
		FIntRect Rect;
		FReadSurfaceDataFlags Flags;
	};

	FReadSurfaceContext Context = {
		TextureResource,
		&ImageRequest->RenderRequest->Image,
		FIntRect(0,0,TextureResource->GetSizeX(), TextureResource->GetSizeY()),
		FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
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
	
	ImageRequest->RenderRequest->RenderFence.BeginFence();
}
