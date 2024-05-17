// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoCamera.h"

#include "TempoCameraServiceSubsystem.h"

#include "TempoCoreSettings.h"
#include "TempoSensorsSettings.h"

#include "Engine/TextureRenderTarget2D.h"

UTempoCamera::UTempoCamera()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	CaptureSource = ESceneCaptureSource::SCS_SceneDepth;
	ShowFlags.SetAntiAliasing(true);
	bCaptureEveryFrame = false;
	bCaptureOnMovement = false;
	bTickInEditor = false;
	bAlwaysPersistRenderingState = true;
}

void UTempoCamera::BeginPlay()
{
	Super::BeginPlay();
	
	// UpdateRenderTarget();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTempoCamera::MaybeCapture, 1.0 / RateHz, true);
}

void UTempoCamera::UpdateSceneCaptureContents(FSceneInterface* Scene)
{
	Super::UpdateSceneCaptureContents(Scene);

	UTempoCameraServiceSubsystem* CameraSubsystem = GetWorld()->GetSubsystem<UTempoCameraServiceSubsystem>();
	if (CameraSubsystem && CameraSubsystem->HasPendingRequestForCamera(CameraId))
	{
		CameraSubsystem->SendImage(CameraId, SequenceId, TextureTarget->GameThread_GetRenderTargetResource());
		SequenceId++;
	}
}

void UTempoCamera::MaybeCapture()
{
	const UTempoCameraServiceSubsystem* CameraSubsystem = GetWorld()->GetSubsystem<UTempoCameraServiceSubsystem>();
	if (TextureTarget && CameraSubsystem && CameraSubsystem->HasPendingRequestForCamera(CameraId))
	{
		if (GetDefault<UTempoCoreSettings>()->GetTimeMode() == ETimeMode::FixedStep)
		{
			// In fixed step mode we block the game thread to render the image immediately.
			// It will then be read and sent before the end of the current frame.
			CaptureScene();
		}
		else
		{
			// Otherwise, we render the frame along with the main render pass.
			// It will get read and sent one or two frames after this one.
			CaptureSceneDeferred();
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
	RenderTarget2D->RenderTargetFormat = ETextureRenderTargetFormat::RTF_R8;
	RenderTarget2D->bGPUSharedFlag = true;
	
	// TextureTarget = RenderTarget2D;
}

void UTempoCamera::UpdatePostProcessMaterial()
{
	// PostProcessSettings.WeightedBlendables.Array.Empty();
	// switch (ImageType)
	// {
	// case EImageType::COLOR:
	// 	{
	// 		PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,
	// 			GetDefault<UTempoSensorsSettings>()->GetColorPostProcessMaterial()));
	// 		break;
	// 	}
	// case EImageType::DEPTH_AND_LABELS:
	// 	{
	// 		PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,
	// 			GetDefault<UTempoSensorsSettings>()->GetDepthAndLabelsPostProcessMaterial()));
	// 		break;
	// 	}
	// default:
	// 	checkf(false, TEXT("Unhandled ImageType"));
	// }
}

#if WITH_EDITOR
void UTempoCamera::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_CHECKED(UTempoCamera, SizeXY))
	{
		UpdateRenderTarget();
	}
	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_CHECKED(UTempoCamera, ImageType))
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdatePostProcessMaterial"));
		UpdatePostProcessMaterial();
	}
}
#endif
