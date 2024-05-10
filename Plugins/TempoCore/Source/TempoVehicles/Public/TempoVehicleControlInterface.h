// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TempoVehicleControlInterface.generated.h"

USTRUCT(BlueprintType)
struct FDrivingInput {
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration = 0.0; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Steering = 0.0; 
};

UINTERFACE()
class UTempoVehicleControlInterface : public UInterface
{
	GENERATED_BODY()
};

class TEMPOVEHICLES_API ITempoVehicleControlInterface
{
	GENERATED_BODY()
	
public:
	virtual int32 GetVehicleId() = 0;
	
	virtual void HandleDrivingInput(const FDrivingInput& Input) = 0;
};
