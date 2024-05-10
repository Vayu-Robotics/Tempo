// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoCamera.h"

#include "TempoCameraServiceSubsystem.h"

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
	if (TextureTarget)
	{
		if (UTempoCameraServiceSubsystem* CameraSubsystem = GetWorld()->GetSubsystem<UTempoCameraServiceSubsystem>())
		{
			CameraSubsystem->AddCameraFrame(CameraId, TextureTarget->GameThread_GetRenderTargetResource());
		}
	}
}
