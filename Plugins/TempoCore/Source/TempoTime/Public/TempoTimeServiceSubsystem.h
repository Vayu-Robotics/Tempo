// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include <grpcpp/grpcpp.h>

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TempoTimeServiceSubsystem.generated.h"

namespace Tempo
{
	class Empty;
}

/**
 * Registers Tempo Time Service.
 */
UCLASS()
class TEMPOTIME_API UTempoTimeServiceSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	grpc::Status Play(const Tempo::Empty& Request, Tempo::Empty& Response) const;

	grpc::Status Pause(const Tempo::Empty& Request, Tempo::Empty& Response) const;
};
