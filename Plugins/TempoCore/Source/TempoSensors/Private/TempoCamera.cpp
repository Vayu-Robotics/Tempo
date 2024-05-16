// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoCamera.h"

#include "TempoCameraServiceSubsystem.h"

#include "TempoCoreSettings.h"

#include "Engine/TextureRenderTarget2D.h"

UTempoCamera::UTempoCamera()
{
	PrimaryComponentTick.bCanEverTick = true;
	CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	ShowFlags.SetAntiAliasing(true);
	bCaptureEveryFrame = false;
	bCaptureOnMovement = false;
	bTickInEditor = false;
	bAlwaysPersistRenderingState = true;
}

void UTempoCamera::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateRenderTarget();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTempoCamera::MaybeCapture, 1.0 / RateHz, true);
}

// void UTempoCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
// {
// 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
// 	UE_LOG(LogTemp, Warning, TEXT("Tick for frame %d from camera %d at %f"), FrameCounter, CameraId, GetWorld()->GetTimeSeconds());
// }

void UTempoCamera::UpdateSceneCaptureContents(FSceneInterface* Scene)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateSceneCaptureContents for frame %d from camera %d at %f"), FrameCounter, CameraId, GetWorld()->GetTimeSeconds());

	Super::UpdateSceneCaptureContents(Scene);

	UTempoCameraServiceSubsystem* CameraSubsystem = GetWorld()->GetSubsystem<UTempoCameraServiceSubsystem>();
	
	if (CameraSubsystem && CameraSubsystem->HasPendingRequestForCamera(CameraId))
	{
		CameraSubsystem->SendImage(CameraId, FrameCounter, TextureTarget->GameThread_GetRenderTargetResource());
		FrameCounter++;
	}

	// bCaptureQueued = false;
}

void UTempoCamera::MaybeCapture()
{
	UE_LOG(LogTemp, Warning, TEXT("MaybeCapture for frame %d from Camera %d at %f"), FrameCounter, CameraId, GetWorld()->GetTimeSeconds(), GetWorld()->WorldType);

	UTempoCameraServiceSubsystem* CameraSubsystem = GetWorld()->GetSubsystem<UTempoCameraServiceSubsystem>();
	
	if (!CameraSubsystem || !CameraSubsystem->HasPendingRequestForCamera(CameraId))
	{
		return;
	}

	if (TextureTarget)
	{
		if (GetDefault<UTempoCoreSettings>()->GetTimeMode() == ETimeMode::FixedStep)
		{
			CaptureScene();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CaptureSceneDeferred for frame %d from Camera %d at %f"), FrameCounter, CameraId, GetWorld()->GetTimeSeconds(), GetWorld()->WorldType);
			if (GetWorld()->WorldType == EWorldType::PIE || GetWorld()->WorldType == EWorldType::Game)
			{
				CaptureSceneDeferred();
				// bCaptureQueued = true;
			}
		}
	}
}

void UTempoCamera::SetSizeXY(const FIntPoint& SizeXYIn)
{
	SizeXY = SizeXYIn;

	UpdateRenderTarget();
}

void UTempoCamera::SetImageType(EImageType ImageTypeIn)
{
	ImageType = ImageTypeIn;

	UpdatePostProcessMaterial();
	UpdateRenderTarget();
}

void UTempoCamera::UpdateRenderTarget()
{
	UTextureRenderTarget2D* RenderTarget2D = NewObject<UTextureRenderTarget2D>(this);

	RenderTarget2D->TargetGamma = GEngine->GetDisplayGamma();
	RenderTarget2D->InitAutoFormat(256, 256);
	RenderTarget2D->InitCustomFormat(SizeXY.X, SizeXY.Y, PF_B8G8R8A8, true);
	RenderTarget2D->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
	RenderTarget2D->bGPUSharedFlag = true;

	TextureTarget = RenderTarget2D;
}

void UTempoCamera::UpdatePostProcessMaterial()
{
	
}

#if WITH_EDITOR
void UTempoCamera::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_CHECKED(UTempoCamera, UTempoCamera::SizeXY))
	{
		UpdateRenderTarget();
	}
	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_CHECKED(UTempoCamera, UTempoCamera::ImageType))
	{
		UpdateRenderTarget();
		UpdatePostProcessMaterial();
	}
}
#endif