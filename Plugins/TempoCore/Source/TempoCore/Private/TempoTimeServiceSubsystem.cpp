// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoTimeServiceSubsystem.h"

#include "time.grpc.pb.h"

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"

using TimeService = Tempo::Time::AsyncService;

void UTempoTimeServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (!GetWorld()->IsGameWorld())
	{
		return;
	}
	
	GetWorld()->GetSubsystem<UTempoScriptingWorldSubsystem>()->GetScriptingServer()->RegisterService<TimeService>(
		TRequestHandler<TimeService, Tempo::Empty, Tempo::Empty>(&TimeService::RequestPlay).BindUObject(this, &UTempoTimeServiceSubsystem::Play),
		TRequestHandler<TimeService, Tempo::Empty, Tempo::Empty>(&TimeService::RequestPause).BindUObject(this, &UTempoTimeServiceSubsystem::Pause)
	);
}

grpc::Status UTempoTimeServiceSubsystem::Play(const Tempo::Empty& Request, Tempo::Empty& Response) const
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetPause(false);
	
	return grpc::Status::OK;
}

grpc::Status UTempoTimeServiceSubsystem::Pause(const Tempo::Empty& Request, Tempo::Empty& Response) const
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetPause(true);
	
	return grpc::Status::OK;
}
