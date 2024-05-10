// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TempoVehicleMovementInterface.generated.h"

USTRUCT(BlueprintType)
struct FDrivingCommand {
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration = 0.0; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Steering = 0.0; 
};

UINTERFACE()
class UTempoVehicleMovementInterface : public UInterface
{
	GENERATED_BODY()
};

class TEMPOVEHICLES_API ITempoVehicleMovementInterface
{
	GENERATED_BODY()

public:
	virtual void HandleDrivingCommand(const FDrivingCommand& Command) = 0;
};
