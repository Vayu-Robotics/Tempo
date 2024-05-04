// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoScriptable.h"

#include "TempoScriptingServer.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StreamedCamera.generated.h"

namespace TempoScripting
{
	class Empty;
}

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEMPO_API UStreamedCamera : public UActorComponent, public ITempoWorldScriptable
{
	GENERATED_BODY()

public:
	UStreamedCamera();

	virtual void BeginPlay() override;

	virtual void RegisterWorldServices(UTempoScriptingServer* ScriptingServer) override;

private:
	UFUNCTION()
	void SendImage();
	
	void StreamCameraImages(const TempoScripting::Empty& Request, const ResponseContinuationType<TempoScripting::Empty>& ResponseContinuation);

	TOptional<ResponseContinuationType<TempoScripting::Empty>> PendingResponseContinuation;

	FTimerHandle TimerHandle;
};
