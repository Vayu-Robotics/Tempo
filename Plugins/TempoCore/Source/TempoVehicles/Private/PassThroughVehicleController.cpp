// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "PassThroughVehicleController.h"

#include "TempoVehicleMovementInterface.h"

void APassThroughVehicleController::HandleDrivingInput(const FDrivingInput& Input)
{
	if (!GetPawn())
	{
		return;
	}
	
	TArray<UActorComponent*> VehicleMovementComponents = GetPawn()->GetComponentsByInterface(UTempoVehicleMovementInterface::StaticClass());
	check(VehicleMovementComponents.Num() == 1);
	Cast<ITempoVehicleMovementInterface>(VehicleMovementComponents[0])->HandleDrivingCommand(FDrivingCommand{ Input.Acceleration, Input.Steering });
}
