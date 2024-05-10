// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoVehicleControlInterface.h"

#include "AIController.h"
#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "PassThroughVehicleController.generated.h"

UCLASS()
class TEMPOVEHICLES_API APassThroughVehicleController : public AAIController, public ITempoVehicleControlInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetVehicleId() override { return VehicleId; }
	
	virtual void HandleDrivingInput(const FDrivingInput& Input) override;

private:
	UPROPERTY(EditAnywhere)
	int32 VehicleId = 0;
};
