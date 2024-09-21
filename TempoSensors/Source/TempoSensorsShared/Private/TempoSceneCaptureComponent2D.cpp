// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoSceneCaptureComponent2D.h"

#include "TempoSensorsSettings.h"

#include "Engine/TextureRenderTarget2D.h"

// FGPUFenceRHIRef UTempoSceneCaptureComponent2D::RenderFence;

UTempoSceneCaptureComponent2D::UTempoSceneCaptureComponent2D()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	ShowFlags.SetAntiAliasing(true);
	bCaptureEveryFrame = false;
	bCaptureOnMovement = false;
	bTickInEditor = false;
	bAlwaysPersistRenderingState = true;
}

void UTempoSceneCaptureComponent2D::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTempoSceneCaptureComponent2D::MaybeCapture, 1.0 / RateHz, true);
}

void UTempoSceneCaptureComponent2D::OnFrameRenderCompleted()
{
	// can we while(!Poll()) instead?
	// if (!RenderFence.IsValid() || !RenderFence->Poll())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Render not complete %s num: %d sequence id: %d"), *GetName(), TextureReadQueue.GetNumPendingTextureReads(), SequenceId);
	// 	return;
	// }
	if (!RenderFence.IsValid())
	{
		return;
	}
	while (!RenderFence->Poll())
	{
		FPlatformProcess::Sleep(1e-4);
	}
	RenderFence.SafeRelease();
	
	UE_LOG(LogTemp, Warning, TEXT("Normal OnRenderFrameCompleted %s num: %d sequence id: %d"), *GetName(), TextureReadQueue.GetNumPendingTextureReads(), SequenceId);
	const FRenderTarget* RenderTarget = TextureTarget->GetRenderTargetResource();
	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("RenderTarget is not initialized"));
		TextureReadQueue.SkipNextPendingTextureRead();
		return;
	}
	if (!TextureRHICopy.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("TextureRHICopy is not valid"));
		TextureReadQueue.SkipNextPendingTextureRead();
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("BeginNextTextureRead %s num: %d sequence id: %d"), *GetName(), TextureReadQueue.GetNumPendingTextureReads(), SequenceId);
	TextureReadQueue.BeginNextPendingTextureRead(RenderTarget, TextureRHICopy);
}

FString UTempoSceneCaptureComponent2D::GetOwnerName() const
{
	check(GetOwner());

	return GetOwner()->GetActorNameOrLabel();
}

FString UTempoSceneCaptureComponent2D::GetSensorName() const
{
	return GetName();
}

void UTempoSceneCaptureComponent2D::UpdateSceneCaptureContents(FSceneInterface* Scene)
{
	ENQUEUE_RENDER_COMMAND(SetTempoCameraRenderFence)(
	[this](FRHICommandList& RHICmdList)
	{
		if (!RenderFence.IsValid())
		{
			RenderFence = RHICreateGPUFence(TEXT("TempoCameraRenderFence"));
			RHICmdList.WriteGPUFence(RenderFence);
		}
	});
	
	Super::UpdateSceneCaptureContents(Scene);
	
	SequenceId++;

	UE_LOG(LogTemp, Warning, TEXT("UpdateSceneCaptureContents %s sequence id: %d"), *GetName(), SequenceId);
}

void UTempoSceneCaptureComponent2D::MaybeCapture()
{
	if (!HasPendingRequests())
	{
		return;
	}

	CaptureSceneDeferred();
}

void UTempoSceneCaptureComponent2D::InitRenderTarget()
{
	TextureTarget = NewObject<UTextureRenderTarget2D>(this);

	TextureTarget->TargetGamma = GetDefault<UTempoSensorsSettings>()->GetSceneCaptureGamma();
	TextureTarget->RenderTargetFormat = RenderTargetFormat;
	TextureTarget->bGPUSharedFlag = true;
	if (PixelFormatOverride == EPixelFormat::PF_Unknown)
	{
		TextureTarget->InitAutoFormat(SizeXY.X, SizeXY.Y);
	}
	else
	{
		TextureTarget->InitCustomFormat(SizeXY.X, SizeXY.Y, PixelFormatOverride, true);
	}

	struct FInitCPUCopyContext {
		FString Name;
		int32 SizeX;
		int32 SizeY;
		EPixelFormat PixelFormat;
		FTextureRHIRef* TextureRHICopy;
	};

	FInitCPUCopyContext Context = {
		FString::Printf(TEXT("%s TextureRHICopy"), *GetName()),
		TextureTarget->SizeX,
		TextureTarget->SizeY,
		TextureTarget->GetFormat(),
		&TextureRHICopy
	};

	ENQUEUE_RENDER_COMMAND(InitCommand)(
		[Context](FRHICommandList& RHICmdList)
		{
			// Create the TextureRHICopy, where we will copy our TextureTarget's resource before reading it on the CPU.
			constexpr ETextureCreateFlags TexCreateFlags = ETextureCreateFlags::Shared | ETextureCreateFlags::CPUReadback;

			const FRHITextureCreateDesc Desc =
				FRHITextureCreateDesc::Create2D(*Context.Name)
				.SetExtent(Context.SizeX, Context.SizeY)
				.SetFormat(Context.PixelFormat)
				.SetFlags(TexCreateFlags);

			*Context.TextureRHICopy = RHICreateTexture(Desc);
		});
}

TOptional<TFuture<void>> UTempoSceneCaptureComponent2D::FlushMeasurementResponses()
{
	if (TUniquePtr<FTextureRead> TextureRead = TextureReadQueue.DequeuePendingTextureRead())
	{
		UE_LOG(LogTemp, Warning, TEXT("DecodeAndRespond %s at sequence id: %d with sequence id: %d"), *GetName(), SequenceId, TextureRead->SequenceId);
		return DecodeAndRespond(MoveTemp(TextureRead));
	}
	
	return TOptional<TFuture<void>>();
}

void UTempoSceneCaptureComponent2D::FlushPendingRenderingCommands() const
{
	if (TextureReadQueue.HasOutstandingTextureReads())
	{
		UE_LOG(LogTemp, Warning, TEXT("FlushPendingTextureReads %s (num: %d) sequence id: %d"), *GetName(), TextureReadQueue.GetNumPendingTextureReads(), SequenceId);
		TextureReadQueue.FlushPendingTextureReads();
	}
}
