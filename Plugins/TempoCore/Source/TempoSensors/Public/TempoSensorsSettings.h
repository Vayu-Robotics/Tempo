// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TempoSensorsSettings.generated.h"

UCLASS(Config=Game)
class TEMPOSENSORS_API UTempoSensorsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	bool GetBlockOnCameraRender() const { return bBlockOnCameraRender; }
	int32 GetMaxCameraRenderBufferSize() const { return MaxCameraRenderBufferSize; }

private:
	// Whether to block the game thread on camera image . This can slightly decrease the latency from camera trigger
	// to rendering, at the expense of the overall game frame rate.
	UPROPERTY(EditAnywhere, Config, Category="Camera", AdvancedDisplay)
	bool bBlockOnCameraRender = false;

	// The max number of frames per camera to buffer before dropping.
	UPROPERTY(EditAnywhere, Config, Category="Camera", AdvancedDisplay)
	int32 MaxCameraRenderBufferSize = 2;
};
