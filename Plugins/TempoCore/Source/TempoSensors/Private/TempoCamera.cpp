// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoCamera.h"

#include "TempoSensors/Camera.pb.h"

#include "TextureResource.h"
#include "Engine/TextureRenderTarget2D.h"

UTempoCamera::UTempoCamera()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTempoCamera::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTempoCamera::SendImage, RateHz, true);
}

void UTempoCamera::SendImage()
{
	if (PendingImageStreamContinuations.IsEmpty())
	{
		return;
	}

	if (TextureTarget)
	{
		const int32 Width = TextureTarget->SizeX;;
		const int32 Height = TextureTarget->SizeY;;
		TArray ImageData(&FColor::Green, Width * Height);
		TextureTarget->GameThread_GetRenderTargetResource()->ReadPixels(ImageData);
		
		std::vector<uint32> PackedData(Width * Height);
		ParallelFor(ImageData.Num(), [&ImageData, &PackedData](int32 Num)
		{
			PackedData[Num] = ImageData[Num].ToPackedARGB();
		});

		TempoSensors::Image Image;
		
		Image.set_width(Width);
		Image.set_height(Height);
		Image.mutable_pixels()->Assign(PackedData.begin(), PackedData.end());

		TResponseDelegate<TempoSensors::Image> ResponseContinuation;
		while (PendingImageStreamContinuations.Dequeue(ResponseContinuation))
		{
			ResponseContinuation.ExecuteIfBound(Image, grpc::Status_OK);
		}
	}
	else
	{
		TResponseDelegate<TempoSensors::Image> ResponseContinuation;
		while (PendingImageStreamContinuations.Dequeue(ResponseContinuation))
		{
			ResponseContinuation.ExecuteIfBound(TempoSensors::Image(), grpc::Status(grpc::StatusCode::ABORTED, "Camera had no texture target"));
		}
	}

	PendingImageStreamContinuations.Empty();
}
