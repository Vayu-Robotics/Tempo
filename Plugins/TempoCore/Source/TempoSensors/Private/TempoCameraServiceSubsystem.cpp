// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoCameraServiceSubsystem.h"

#include "TempoCamera.h"
#include "TempoSensors.h"
#include "TempoSensorsSettings.h"

#include "TempoSensors/Camera.grpc.pb.h"

#include "TempoCoreSettings.h"

#include "ImageUtils.h"

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

TempoSensors::ImageType ToProtoImageType(EImageType ImageType)
{
	switch (ImageType)
	{
	case EImageType::COLOR:
		{
			return TempoSensors::COLOR;
		}
	case EImageType::DEPTH_AND_LABELS:
		{
			return TempoSensors::DEPTH_AND_LABELS;
		}
	default:
		{
			checkf(false, TEXT("Unhandled image type"));
			return TempoSensors::COLOR;
		}
	}
}

void UTempoCameraServiceSubsystem::GetAvailableCameras(const TempoSensors::AvailableCamerasRequest& Request, const TResponseDelegate<TempoSensors::AvailableCamerasResponse>& ResponseContinuation) const
{
	AvailableCamerasResponse Response;
	for (TObjectIterator<UTempoCamera> CameraIt; CameraIt; ++CameraIt)
	{
		const UTempoCamera* Camera = *CameraIt;
		if (IsValid(Camera) && Camera->GetWorld() == GetWorld())
		{
			auto* AvailableCamera = Response.add_available_cameras();
			AvailableCamera->set_id(Camera->GetId());
			AvailableCamera->set_name(TCHAR_TO_UTF8(*Camera->GetName()));
			AvailableCamera->set_image_type(ToProtoImageType(Camera->GetImageType()));
			AvailableCamera->set_width(Camera->GetSizeXY().X);
			AvailableCamera->set_height(Camera->GetSizeXY().Y);
			AvailableCamera->set_rate(Camera->GetRate());
		}
	}
	ResponseContinuation.ExecuteIfBound(Response, grpc::Status_OK);
}

int32 QualityFromCompressionLevel(TempoSensors::CompressionLevel CompressionLevel)
{
	// See FJpegImageWrapper::Compress() for an explanation of these levels
	switch (CompressionLevel)
	{
	case TempoSensors::MIN:
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
	RequestedCameras.FindOrAdd(Request.camera_id()).PendingImageRequests.Add(FImageRequest(Quality, ResponseContinuation));
}

void UTempoCameraServiceSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// In fixed step mode we block the game thread on any pending texture reads.
	// This guarantees they will be sent out in the same frame when they were captured.
	if (GetDefault<UTempoCoreSettings>()->GetTimeMode() == ETimeMode::FixedStep)
	{
		for (const auto& PendingImageReqeust : RequestedCameras)
		{
			if (PendingImageReqeust.Value.HasOutstandingTextureReads())
			{
				FlushRenderingCommands();
				break;
			}
		}
	}
	
	for (auto& Elem : RequestedCameras)
	{
		FRequestedCamera& RequestedCamera = Elem.Value;
		if (const TUniquePtr<FTextureRead> TextureRead = RequestedCamera.DequeuePendingTextureRead())
		{
			// Cache to avoid duplicated compression for clients who requested the same quality.
			TMap<int32, TArray64<uint8>> CompressedCache;
			for (auto ImageRequest = RequestedCamera.PendingImageRequests.CreateIterator(); ImageRequest; ++ImageRequest)
			{
				TArray64<uint8>& CompressedData = CompressedCache.FindOrAdd(ImageRequest->Quality);
				if (CompressedData.IsEmpty())
				{
					FImageUtils::CompressImage(CompressedData,
						TEXT("jpg"),
						FImageView(TextureRead->Image.GetData(), TextureRead->ImageSize.X, TextureRead->ImageSize.Y),
						ImageRequest->Quality);
				}

				TempoSensors::Image Image;	
				Image.set_width(TextureRead->ImageSize.X);
				Image.set_height(TextureRead->ImageSize.Y);
				Image.set_data(CompressedData.GetData(), CompressedData.Num());
				Image.mutable_header()->set_sequence_id(TextureRead->SequenceId);
				Image.mutable_header()->set_capture_time(TextureRead->CaptureTime);
				Image.mutable_header()->set_transmission_time(GetWorld()->GetTimeSeconds());

				ImageRequest->ResponseContinuation.ExecuteIfBound(Image, grpc::Status_OK);

				ImageRequest.RemoveCurrent();
			}
		}
	}
}

TStatId UTempoCameraServiceSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTempoCameraServiceSubsystem, STATGROUP_Tickables);
}

bool UTempoCameraServiceSubsystem::HasPendingRequestForCamera(int32 CameraId) const
{
	return RequestedCameras.Contains(CameraId) && !RequestedCameras[CameraId].PendingImageRequests.IsEmpty();
}

void UTempoCameraServiceSubsystem::SendImage(int32 CameraId, int32 SequenceId, FTextureRenderTargetResource* TextureResource)
{
	FRequestedCamera* RequestedCamera = RequestedCameras.Find(CameraId);
	if (!RequestedCamera)
	{
		checkf(false, TEXT("Camera %d sent image but was not requested"), CameraId);
	}
	
	if (RequestedCamera->GetNumPendingTextureReads() > GetDefault<UTempoSensorsSettings>()->GetMaxCameraRenderBufferSize())
	{
		UE_LOG(LogTempoSensors, Warning, TEXT("Fell behind while rendering images from camera %d. Dropping image."), CameraId);
		return;
	}

	FTextureRead* TextureRead = new FTextureRead(
		TextureResource->GetSizeXY(),
		SequenceId,
		GetWorld()->GetTimeSeconds());
	
	struct FReadSurfaceContext{
		FRenderTarget* SrcRenderTarget;
		TArray<FColor>* OutData;
		FIntRect Rect;
		FReadSurfaceDataFlags Flags;
	};

	FReadSurfaceContext Context = {
		TextureResource,
		&TextureRead->Image,
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
	
	TextureRead->RenderFence.BeginFence();
	RequestedCamera->EnqueuePendingTextureRead(TextureRead);
}
