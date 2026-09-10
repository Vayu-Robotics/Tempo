// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoDateTimeSystem.h"

#include "TempoGeographic.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkyLightComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "CoreGlobals.h"   // GFrameCounter - TEMP [NaniteCP]

ATempoDateTimeSystem* ATempoDateTimeSystem::GetTempoDateTimeSystem(UObject* WorldContextObject)
{
	ATempoDateTimeSystem* Actor = nullptr;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(World, ATempoDateTimeSystem::StaticClass(), Actors);
		if (Actors.Num() == 0)
		{
			UE_LOG(LogTempoGeographic, Error, TEXT("TempoDateTime actor not found"));
		}
		else if (Actors.Num() > 1)
		{
			UE_LOG(LogTempoGeographic, Error, TEXT("Multiple TempoDateTime actors found"));
		}
		else
		{
			Actor = Cast<ATempoDateTimeSystem>(Actors[0]);
		}
	}

	return Actor;
}

ATempoDateTimeSystem::ATempoDateTimeSystem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATempoDateTimeSystem::Tick(float DeltaSeconds)
{
	// Re-capture the sky at the START of the tick, using the sun position established by the PREVIOUS
	// frame's broadcast. By now every DateTimeChangedEvent listener (including the TempoSunSky Blueprint
	// that rotates the sun and owns the SkyLight) has already run, so the sun is final for that frame.
	// This side-steps the undefined multicast-delegate invocation order: we never listen to the event,
	// we act one frame after it. bSunHasBeenPositioned gates out frame 0, whose sun is still the default.
	if (bSunHasBeenPositioned && bRecaptureSkyOnSunChange)
	{
		TimeSinceLastSkyCapture += DeltaSeconds;
		const bool bIntervalElapsed = SkyRecaptureIntervalSeconds <= 0.0f || TimeSinceLastSkyCapture >= SkyRecaptureIntervalSeconds;
		if (!bHasCapturedSkyOnce || bIntervalElapsed)
		{
			RecaptureStaticSkyLights();
			bHasCapturedSkyOnce = true;
			TimeSinceLastSkyCapture = 0.0f;
		}
	}
	else if (bSunHasBeenPositioned && !bHasCapturedSkyOnce)
	{
		// Periodic recapture disabled: still do a single capture once the sun is positioned.
		RecaptureStaticSkyLights();
		bHasCapturedSkyOnce = true;
	}

	SimDateTime += FTimespan::FromSeconds(DayCycleRelativeRate * DeltaSeconds);

	BroadcastDateTimeChanged();
	bSunHasBeenPositioned = true;
}

void ATempoDateTimeSystem::RecaptureStaticSkyLights()
{
	UWorld* World = GetWorld();
	if (!World || !World->Scene)
	{
		return;
	}

	// Mark every captured-scene sky light owned by this world dirty, forcing it to static capture first.
	int32 NumDirtied = 0;
	for (TObjectIterator<USkyLightComponent> It; It; ++It)
	{
		USkyLightComponent* SkyLight = *It;
		if (!IsValid(SkyLight) || SkyLight->GetWorld() != World)
		{
			continue;
		}

		// We only handle captured-scene sky lights whose owner is actually in this world.
		if (World->ContainsActor(SkyLight->GetOwner()) && SkyLight->SourceType == SLS_CapturedScene)
		{
			// The level's SkyLight ships real-time-capture-enabled (bRealTimeCapture=True on the asset).
			// Real-time capture only runs inside the main deferred view render, which never happens under
			// -RenderOffscreen, so the ambient cubemap stays empty and shadow sides render near-black.
			// Force the light to STATIC capture at runtime so (a) it passes the non-real-time filter below,
			// and (b) the static queue pump USkyLightComponent::UpdateSkyCaptureContents won't RemoveAll it
			// (that pump drops any light where IsRealTimeCaptureEnabled()). This is asset-flag-independent:
			// it self-heals across every L_Gen* level regardless of whether the umap was re-saved static.
			if (SkyLight->IsRealTimeCaptureEnabled())
			{
				SkyLight->SetRealTimeCaptureEnabled(false);
			}

			if (!SkyLight->IsRealTimeCaptureEnabled())
			{
				SkyLight->SetCaptureIsDirty();
				++NumDirtied;
			}
		}
	}

	if (NumDirtied > 0)
	{
		// TEMP [NaniteCP] instrumentation. This pump queues a cubemap capture whose render-thread
		// scene update has asserted one frame later (NaniteShading.cpp:2200, dangling Custom Pass refs
		// on a Nanite raster bin). Log the frame BEFORE the pump: the existing Display line below only
		// prints if the pump returned, so "enter without pumped" localises the fault inside the pump,
		// and the frame number cross-references TempoActorLabeler's [NaniteCP] custom-depth toggles.
		// Strip with Docs/Unreal56/Bugs/nanite_custompass_skylight_capture.md
		UE_LOG(LogTempoGeographic, Warning, TEXT("[NaniteCP] sky pump enter frame=%llu world=%s dirtied=%d begunplay=%d"),
			(uint64)GFrameCounter, *World->GetName(), NumDirtied, World->HasBegunPlay() ? 1 : 0);

		// Pump the static capture queue now. In a normal client this is driven from UGameEngine::Tick,
		// but that pump is skipped under -RenderOffscreen; calling it here makes it run headless.
		USkyLightComponent::UpdateSkyCaptureContents(World);
		UE_LOG(LogTempoGeographic, Display, TEXT("RecaptureStaticSkyLights: pumped %d static sky-light capture(s) in %s"),
			NumDirtied, *World->GetName());
	}
}

void ATempoDateTimeSystem::AdvanceSimDateTime(const FTimespan& Timespan)
{
	SimDateTime += Timespan;

	BroadcastDateTimeChanged();
}

void ATempoDateTimeSystem::BroadcastDateTimeChanged() const
{
	DateTimeChangedEvent.Broadcast(SimDateTime);
}
