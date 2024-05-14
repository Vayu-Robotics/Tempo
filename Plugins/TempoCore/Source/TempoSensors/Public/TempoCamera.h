// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "TempoCamera.generated.h"

namespace TempoSensors
{
	class Image;
}

UCLASS(Blueprintable, BlueprintType)
class TEMPOSENSORS_API UTempoCamera : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:
	UTempoCamera();

	int32 GetId() const { return CameraId; }

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void SendImage();

	UFUNCTION(CallInEditor)
	void RunCompressionTest();
	
	UPROPERTY(EditAnywhere)
	int32 CameraId = 0;

	UPROPERTY(EditAnywhere)
	float RateHz = 1.0;

	FTimerHandle TimerHandle;
};
