// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "BicycleModelMovementComponent.h"

UBicycleModelMovementComponent::UBicycleModelMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBicycleModelMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Current State
	float LinearVelocity = FVector2D(Velocity).Dot(FVector2D(GetOwner()->GetActorForwardVector()));
	const float HeadingAngle = GetOwner()->GetActorRotation().Yaw;

	// Remap and clamp Command (if any).
	float Acceleration = 0.0;
	if (LatestCommand.IsSet())
	{
		const FDrivingCommand& Command = LatestCommand.GetValue();
		LatestCommand.Reset();
		
		SteeringAngle = Command.Steering > 0.0 ?
			FMath::Min(MaxSteerAngle, Command.Steering * MaxSteerAngle) :
			FMath::Max(-MaxSteerAngle, Command.Steering * MaxSteerAngle);
		Acceleration = Command.Acceleration > 0.0 ?
			FMath::Min(MaxAcceleration, Command.Acceleration * MaxAcceleration) : LinearVelocity > 0.0 ?
			FMath::Max(-MaxDeceleration, Command.Acceleration * MaxDeceleration) :
			FMath::Min(-MaxAcceleration, Command.Acceleration * MaxAcceleration) ;
	}
	
	// Update state.
	float DeltaVelocity = DeltaTime * Acceleration;
	if (LinearVelocity > 0.0 && DeltaVelocity < 0.0)
	{
		// If slowing down, don't start reversing.
		DeltaVelocity = FMath::Max(-LinearVelocity, DeltaVelocity);
	}
	LinearVelocity += DeltaVelocity;
	const float Beta = FMath::DegreesToRadians(SteeringAngle + HeadingAngle);
	Velocity = LinearVelocity * FVector(FMath::Cos(Beta), FMath::Sin(Beta), 0.0);
	const float HeadingDot = FMath::RadiansToDegrees(LinearVelocity * FMath::Sin(FMath::DegreesToRadians(SteeringAngle)) / Wheelbase);

	GetOwner()->AddActorWorldOffset(FVector(DeltaTime * Velocity));
	GetOwner()->SetActorRotation(FRotator(0.0, DeltaTime * HeadingDot, 0.0));
}

void UBicycleModelMovementComponent::HandleDrivingCommand(const FDrivingCommand& Command)
{
	LatestCommand = Command;
}
