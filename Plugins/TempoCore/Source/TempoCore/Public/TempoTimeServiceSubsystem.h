// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TempoScriptingWorldSubsystem.h"

#include "TempoTimeServiceSubsystem.generated.h"

namespace Tempo
{
	class Empty;
}

/**
 * Registers Tempo Time Service.
 */
UCLASS()
class TEMPOSCRIPTING_API UTempoTimeServiceSubsystem : public UTempoScriptingWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	grpc::Status Play(const Tempo::Empty& Request, Tempo::Empty& Response) const;

	grpc::Status Pause(const Tempo::Empty& Request, Tempo::Empty& Response) const;
};
