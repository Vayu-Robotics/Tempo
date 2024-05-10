// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoVehicleMovementInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"

#include "BicycleModelMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEMPOVEHICLES_API UBicycleModelMovementComponent : public UPawnMovementComponent, public ITempoVehicleMovementInterface
{
	GENERATED_BODY()

public:
	UBicycleModelMovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
						   FActorComponentTickFunction* ThisTickFunction) override;

	virtual void HandleDrivingCommand(const FDrivingCommand& Command) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Wheelbase = 100.0; // CM

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAcceleration = 200.0; // CM/S/S (~0.2g)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDeceleration = 1000.0; // CM/S/S (~1.0g) 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSteerAngle = 10.0; // Degrees, symmetric
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SteeringAngle = 0.0; // Degrees
	
	TOptional<FDrivingCommand> LatestCommand;
};
