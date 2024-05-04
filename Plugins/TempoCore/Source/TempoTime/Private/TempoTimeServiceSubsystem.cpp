// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoTimeServiceSubsystem.h"

#include "TempoTimeWorldSettings.h"
#include "TempoTime/Time.grpc.pb.h"

#include "TempoScriptingWorldSubsystem.h"

using TimeService = TempoTime::TimeService::AsyncService;
using TempoEmpty = TempoScripting::Empty;

void UTempoTimeServiceSubsystem::RegisterWorldServices(UTempoScriptingServer* ScriptingServer)
{
	ScriptingServer->RegisterService<TimeService>(
		TSimpleRequestHandler<TimeService, TempoEmpty, TempoEmpty>(&TimeService::RequestPlay).BindUObject(this, &UTempoTimeServiceSubsystem::Play),
		TSimpleRequestHandler<TimeService, TempoEmpty, TempoEmpty>(&TimeService::RequestPause).BindUObject(this, &UTempoTimeServiceSubsystem::Pause)
		);
}

void UTempoTimeServiceSubsystem::Play(const TempoEmpty& Request, const ResponseContinuationType<TempoEmpty>& ResponseContinuation) const
{
	if (ATempoTimeWorldSettings* WorldSettings = Cast<ATempoTimeWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		WorldSettings->SetPaused(false);
	}
	
	ResponseContinuation(TempoEmpty(), grpc::Status());
}

void UTempoTimeServiceSubsystem::Pause(const TempoEmpty& Request, const ResponseContinuationType<TempoEmpty>& ResponseContinuation) const
{
	if (ATempoTimeWorldSettings* WorldSettings = Cast<ATempoTimeWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		WorldSettings->SetPaused(true);
	}
	
	ResponseContinuation(TempoEmpty(), grpc::Status());
}
