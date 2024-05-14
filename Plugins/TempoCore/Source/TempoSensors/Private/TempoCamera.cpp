// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoCamera.h"

#include "TempoCameraServiceSubsystem.h"

#include "Engine/TextureRenderTarget2D.h"

#include "ImageUtils.h"

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
	if (TextureTarget)
	{
		if (UTempoCameraServiceSubsystem* CameraSubsystem = GetWorld()->GetSubsystem<UTempoCameraServiceSubsystem>())
		{
			CameraSubsystem->MaybeSendImage(CameraId, TextureTarget->GameThread_GetRenderTargetResource());
		}
	}
}

void UTempoCamera::RunCompressionTest()
{
	const int32 SizeX = TextureTarget->GameThread_GetRenderTargetResource()->GetSizeX();
	const int32 SizeY = TextureTarget->GameThread_GetRenderTargetResource()->GetSizeY();
	TArray<FColor> ImageData;
	ImageData.Reserve(SizeX * SizeY);
	TextureTarget->GameThread_GetRenderTargetResource()->ReadPixels(ImageData);

	const float UncompressedSize = SizeX * SizeY * 4.0;
	constexpr int32 Repeat = 10;
	for (int32 Quality = 100; Quality >= 40; Quality -= 15)
	{
		const double Start = FPlatformTime::Seconds();
		TArray64<uint8> CompressedData;
		for (int32 Iter = 0; Iter < Repeat; ++Iter)
		{
			FImageUtils::CompressImage(CompressedData, TEXT("jpg"), FImageView(ImageData.GetData(), SizeX, SizeY), Quality);
		}
		UE_LOG(LogTemp, Warning, TEXT("Quality: %d Compression: %f Time: %f"), Quality, CompressedData.Num() / UncompressedSize, (FPlatformTime::Seconds() - Start) / 10.0);
	}
}
