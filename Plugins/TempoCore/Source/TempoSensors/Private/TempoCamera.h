// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoSensorsTypes.h"

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "TempoCamera.generated.h"

UCLASS(Blueprintable, BlueprintType)
class TEMPOSENSORS_API UTempoCamera : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:
	UTempoCamera();

	int32 GetId() const { return CameraId; }

	const FIntPoint& GetSizeXY() const { return SizeXY; }

	float GetRate() const { return RateHz; }

	EImageType GetImageType() const { return ImageType; }

	void SetSizeXY(const FIntPoint& SizeXYIn);

	void SetImageType(EImageType ImageTypeIn);
	
	virtual void UpdateSceneCaptureContents(FSceneInterface* Scene) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void MaybeCapture();
	
	void UpdateRenderTarget();

	void UpdatePostProcessMaterial();
	
	UPROPERTY(EditAnywhere)
	int32 CameraId = 0;

	UPROPERTY(EditAnywhere)
	int32 FrameCounter = 0;
	
	UPROPERTY(EditAnywhere)
	float RateHz = 10.0;

	UPROPERTY(EditAnywhere)
	FIntPoint SizeXY = FIntPoint(960, 540);

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EImageType> ImageType = EImageType::RGB;
	
	FTimerHandle TimerHandle;
};
