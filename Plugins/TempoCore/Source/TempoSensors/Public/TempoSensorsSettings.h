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
	// Labels
	TObjectPtr<UDataTable> GetSemanticLabelTable() const { return SemanticLabelTable.Get(); }

	// Camera
	TObjectPtr<UObject> GetColorPostProcessMaterial() const { return ColorPostProcessMaterial.Get(); }
	TObjectPtr<UObject> GetDepthAndLabelsPostProcessMaterial() const { return DepthAndLabelsPostProcessMaterial.Get(); }
	int32 GetMaxCameraRenderBufferSize() const { return MaxCameraRenderBufferSize; }

private:
	UPROPERTY(EditAnywhere, Config, Category="Labels")
	TSoftObjectPtr<UDataTable> SemanticLabelTable;

	UPROPERTY(EditAnywhere, Config, Category="Camera", meta=( AllowedClasses="/Script/Engine.BlendableInterface"))
	TSoftObjectPtr<UMaterialInterface> ColorPostProcessMaterial;
	
	UPROPERTY(EditAnywhere, Config, Category="Camera", meta=( AllowedClasses="/Script/Engine.BlendableInterface"))
	TSoftObjectPtr<UMaterialInterface> DepthAndLabelsPostProcessMaterial;

	// The max number of frames per camera to buffer before dropping.
	UPROPERTY(EditAnywhere, Config, Category="Camera", AdvancedDisplay)
	int32 MaxCameraRenderBufferSize = 2;
};
