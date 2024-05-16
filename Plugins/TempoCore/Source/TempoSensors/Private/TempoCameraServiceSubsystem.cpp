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
	case EImageType::RGB:
		{
			return TempoSensors::RBG;
		}
	case EImageType::DEPTH:
		{
			return TempoSensors::DEPTH;
		}
	case EImageType::SEMANTIC_LABELS:
		{
			return TempoSensors::SEMANTIC_LABELS;
		}
	default:
		{
			checkf(false, TEXT("Unhandled image type"));
			return TempoSensors::RBG;
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

	// In some cases (always in synchronous mode or when configured to do so in asynchronous mode) we block the
	// game thread to flush all rendering commands and send the images at the first available opportunity.
	const UTempoSensorsSettings* TempoSensorsSettings = GetDefault<UTempoSensorsSettings>();
	const UTempoCoreSettings* TempoCoreSettings = GetDefault<UTempoCoreSettings>();
	if (TempoSensorsSettings->GetBlockOnCameraRender() || TempoCoreSettings->GetTimeMode() == ETimeMode::FixedStep)
	{
		for (const auto& PendingRenderRequest : PendingRenderRequests)
		{
			if (!PendingRenderRequest.Value.IsEmpty() && !PendingRenderRequest.Value[0]->RenderFence.IsFenceComplete())
			{
				FlushRenderingCommands();
				break;
			}
		}
	}
	
	for (auto PendingImageRequestIt = PendingImageRequests.CreateIterator(); PendingImageRequestIt; ++PendingImageRequestIt)
	{
		const int32 CameraId = PendingImageRequestIt->Key;
		FImageRequest& ImageRequest = PendingImageRequestIt->Value;
		if (auto* RenderRequestBuffer = PendingRenderRequests.Find(CameraId))
		{
			while (!RenderRequestBuffer->IsEmpty() && (*RenderRequestBuffer)[0]->RenderFence.IsFenceComplete())
			{
				const TUniquePtr<FRenderRequest> RenderRequest((*RenderRequestBuffer)[0].Release());
				RenderRequestBuffer->RemoveAt(0);

				UE_LOG(LogTemp, Warning, TEXT("Sending frame %d from Camera %d at %f"), RenderRequest->FrameCounter, CameraId, GetWorld()->GetTimeSeconds());
				
				// Compress
				TArray64<uint8> CompressedData;
				FImageUtils::CompressImage(CompressedData,
							TEXT("jpg"),
							FImageView(RenderRequest->Image.GetData(), RenderRequest->ImageSize.X, RenderRequest->ImageSize.Y),
							ImageRequest.Quality);

				// Pack into message
				TempoSensors::Image Image;	
				Image.set_width(RenderRequest->ImageSize.X);
				Image.set_height(RenderRequest->ImageSize.Y);
				Image.set_data(CompressedData.GetData(), CompressedData.Num());
				Image.set_sequence_id(RenderRequest->FrameCounter);
				// Image.set_capture_time()

				// Send
				ImageRequest.ResponseContinuation.ExecuteIfBound(Image, grpc::Status_OK);

				PendingImageRequests.Remove(CameraId);
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
	return PendingImageRequests.Contains(CameraId);
}

void UTempoCameraServiceSubsystem::SendImage(int32 CameraId, int32 FrameCounter, FTextureRenderTargetResource* TextureResource)
{
	checkf(PendingImageRequests.Contains(CameraId), TEXT("Camera %d sent image but was not requested"), CameraId);

	const UTempoSensorsSettings* TempoSensorsSettings = GetDefault<UTempoSensorsSettings>();
	TArray<TUniquePtr<FRenderRequest>>& RenderRequestBuffer = PendingRenderRequests.FindOrAdd(CameraId);
	if (RenderRequestBuffer.Num() > TempoSensorsSettings->GetMaxCameraRenderBufferSize())
	{
		UE_LOG(LogTempoSensors, Warning, TEXT("Fell behind while rendering images from camera %d. Dropping image."), CameraId);
		return;
	}

	FRenderRequest* RenderRequest = new FRenderRequest(TextureResource->GetSizeXY(), FrameCounter);
	
	struct FReadSurfaceContext{
		FRenderTarget* SrcRenderTarget;
		TArray<FColor>* OutData;
		FIntRect Rect;
		FReadSurfaceDataFlags Flags;
	};

	FReadSurfaceContext Context = {
		TextureResource,
		&RenderRequest->Image,
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
	
	RenderRequest->RenderFence.BeginFence();
	RenderRequestBuffer.Emplace(RenderRequest);
}
