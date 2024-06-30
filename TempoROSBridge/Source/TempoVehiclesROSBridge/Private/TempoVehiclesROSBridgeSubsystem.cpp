// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoVehiclesROSBridgeSubsystem.h"

#include "TempoVehiclesROSConverters.h"

#include "TempoROSNode.h"

void UTempoVehiclesROSBridgeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	if (InWorld.WorldType != EWorldType::Game && InWorld.WorldType != EWorldType::PIE)
	{
		return;
	}

	ROSNode = UTempoROSNode::Create("TempoVehicles", this, &InWorld);

	ROSNode->AddService<FTempoGetCommandableVehiclesService>(
		"GetCommandableVehicles",
		TROSServiceDelegate<FTempoGetCommandableVehiclesService>::CreateLambda(
			[this](const FTempoGetCommandableVehiclesService::Request& Request)
		{
			TOptional<FTempoGetCommandableVehiclesService::Response> Response;
			GetCommandableVehicles(
				TempoScripting::Empty(),
				TResponseDelegate<TempoVehicles::CommandableVehiclesResponse>::CreateLambda(
					[&Response](const TempoVehicles::CommandableVehiclesResponse& ResponseIn, grpc::Status Status)
			{
				Response = ResponseIn;
			}));
			checkf(Response.IsSet(), TEXT("GetCommandableVehicles service did not generate response immediately"));
			return Response.GetValue();
		}));

	ROSNode->AddService<FTempoCommandVehicleService>(
	"CommandVehicle",
	TROSServiceDelegate<FTempoCommandVehicleService>::CreateLambda(
		[this](const FTempoCommandVehicleService::Request& Request)
		{
			TOptional<FTempoCommandVehicleService::Response> Response;
			HandleDrivingCommand(
				Request,
				TResponseDelegate<TempoScripting::Empty>::CreateLambda(
					[&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
			{
				Response = FTempoCommandVehicleService::Response();
			}));
			checkf(Response.IsSet(), TEXT("CommandVehicle service did not generate response immediately"));
			return Response.GetValue();
		}));
}
