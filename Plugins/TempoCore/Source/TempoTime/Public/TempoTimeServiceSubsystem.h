// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoScriptable.h"

#include "TempoScriptingServer.h"

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TempoTimeServiceSubsystem.generated.h"

namespace TempoScripting
{
	class Empty;
}

/**
 * Registers Tempo Time Service.
 */
UCLASS()
class TEMPOTIME_API UTempoTimeServiceSubsystem : public UWorldSubsystem, public ITempoWorldScriptable
{
	GENERATED_BODY()
	
public:
	virtual void RegisterWorldServices(UTempoScriptingServer* ScriptingServer) override;

private:
	void Play(const TempoScripting::Empty& Request, const ResponseContinuationType<TempoScripting::Empty>& ResponseContinuation) const;

	void Pause(const TempoScripting::Empty& Request, const ResponseContinuationType<TempoScripting::Empty>& ResponseContinuation) const;
};
