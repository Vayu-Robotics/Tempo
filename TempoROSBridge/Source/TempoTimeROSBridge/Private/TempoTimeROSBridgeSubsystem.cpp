// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoTimeROSBridgeSubsystem.h"

#include "TempoTimeROSConverters.h"
#include "TempoScriptingROSConverters.h"

#include "TempoROSNode.h"

void UTempoTimeROSBridgeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	if (InWorld.WorldType != EWorldType::Game && InWorld.WorldType != EWorldType::PIE)
	{
		return;
	}

	ROSNode = UTempoROSNode::Create("TempoTime", this, &InWorld);

	ROSNode->AddService<FTempoAdvanceStepsService>("AdvanceSteps", TROSServiceDelegate<FTempoAdvanceStepsService>::CreateLambda([this](const FTempoAdvanceStepsService::Request& Request)
	{
		TOptional<FTempoAdvanceStepsService::Response> Response;
		AdvanceSteps(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = FTempoAdvanceStepsService::Response();
		}));
		checkf(Response.IsSet(), TEXT("AdvanceSteps service did not generate response immediately"));
		return Response.GetValue();
	}));

	ROSNode->AddService<FTempoSetSimStepsPerSecondService>("SetSimStepsPerSecond", TROSServiceDelegate<FTempoSetSimStepsPerSecondService>::CreateLambda([this](const FTempoSetSimStepsPerSecondService::Request& Request)
	{
		TOptional<FTempoSetSimStepsPerSecondService::Response> Response;
		SetSimStepsPerSecond(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = FTempoSetSimStepsPerSecondService::Response();
		}));
		checkf(Response.IsSet(), TEXT("SetSimStepsPerSecond service did not generate response immediately"));
		return Response.GetValue();
	}));

	ROSNode->AddService<FTempoSetTimeModeService>("SetTimeMode", TROSServiceDelegate<FTempoSetTimeModeService>::CreateLambda([this](const FTempoSetTimeModeService::Request& Request)
	{
		TOptional<FTempoSetTimeModeService::Response> Response;
		SetTimeMode(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = FTempoSetTimeModeService::Response();
		}));
		checkf(Response.IsSet(), TEXT("SetTimeMode service did not generate response immediately"));
		return Response.GetValue();
	}));

	ROSNode->AddService<FTempoEmptyService>("Play", TROSServiceDelegate<FTempoEmptyService>::CreateLambda([this](const FTempoEmptyService::Request& Request)
	{
		TOptional<FTempoEmptyService::Response> Response;
		Play(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = ResponseIn;
		}));
		checkf(Response.IsSet(), TEXT("Play service did not generate response immediately"));
		return Response.GetValue();
	}));

	ROSNode->AddService<FTempoEmptyService>("Pause", TROSServiceDelegate<FTempoEmptyService>::CreateLambda([this](const FTempoEmptyService::Request& Request)
	{
		TOptional<FTempoEmptyService::Response> Response;
		Pause(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = ResponseIn;
		}));
		checkf(Response.IsSet(), TEXT("Pause service did not generate response immediately"));
		return Response.GetValue();
	}));

	ROSNode->AddService<FTempoEmptyService>("Step", TROSServiceDelegate<FTempoEmptyService>::CreateLambda([this](const FTempoEmptyService::Request& Request)
	{
		TOptional<FTempoEmptyService::Response> Response;
		Step(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = ResponseIn;
		}));
		checkf(Response.IsSet(), TEXT("Step service did not generate response immediately"));
		return Response.GetValue();
	}));
}
