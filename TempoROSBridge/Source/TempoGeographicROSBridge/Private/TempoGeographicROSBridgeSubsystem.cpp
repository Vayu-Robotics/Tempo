// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoGeographicROSBridgeSubsystem.h"

#include "TempoGeographicROSConverters.h"

#include "TempoROSNode.h"

void UTempoGeographicROSBridgeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	if (InWorld.WorldType != EWorldType::Game && InWorld.WorldType != EWorldType::PIE)
	{
		return;
	}

	ROSNode = UTempoROSNode::Create("TempoGeographic", this, &InWorld);

	ROSNode->AddService<FTempoSetDayCycleRateService>("SetDayCycleRate", TROSServiceDelegate<FTempoSetDayCycleRateService>::CreateLambda([this](const FTempoSetDayCycleRateService::Request& Request)
	{
		TOptional<FTempoSetDayCycleRateService::Response> Response;
		SetDayCycleRelativeRate(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = FTempoSetDayCycleRateService::Response();
		}));
		checkf(Response.IsSet(), TEXT("SetDayCycleRate service did not generate response immediately"));
		return Response.GetValue();
	}));

	ROSNode->AddService<FTempoSetTimeOfDayService>("SetTimeOfDay", TROSServiceDelegate<FTempoSetTimeOfDayService>::CreateLambda([this](const FTempoSetTimeOfDayService::Request& Request)
	{
		TOptional<FTempoSetTimeOfDayService::Response> Response;
		SetTimeOfDay(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = FTempoSetTimeOfDayService::Response();
		}));
		checkf(Response.IsSet(), TEXT("SetTimeOfDay service did not generate response immediately"));
		return Response.GetValue();
	}));

	ROSNode->AddService<FTempoSetDayCycleRateService>("SetDayCycleRate", TROSServiceDelegate<FTempoSetDayCycleRateService>::CreateLambda([this](const FTempoSetDayCycleRateService::Request& Request)
	{
		TOptional<FTempoSetDayCycleRateService::Response> Response;
		SetDayCycleRelativeRate(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = FTempoSetDayCycleRateService::Response();
		}));
		checkf(Response.IsSet(), TEXT("SetDayCycleRate service did not generate response immediately"));
		return Response.GetValue();
	}));

	ROSNode->AddService<FTempoSetGeographicReferenceService>("SetGeographicReference", TROSServiceDelegate<FTempoSetGeographicReferenceService>::CreateLambda([this](const FTempoSetGeographicReferenceService::Request& Request)
	{
		TOptional<FTempoSetGeographicReferenceService::Response> Response;
		SetGeographicReference(Request, TResponseDelegate<TempoScripting::Empty>::CreateLambda([&Response](const TempoScripting::Empty& ResponseIn, grpc::Status Status)
		{
			Response = FTempoSetGeographicReferenceService::Response();
		}));
		checkf(Response.IsSet(), TEXT("SetGeographicReference service did not generate response immediately"));
		return Response.GetValue();
	}));

	ROSNode->AddService<FTempoGetDateTimeService>("GetDateTime", TROSServiceDelegate<FTempoGetDateTimeService>::CreateLambda([this](const FTempoGetDateTimeService::Request& Request)
	{
		TOptional<TempoGeographic::DateTime> Response;
		GetDateTime(TempoScripting::Empty(), TResponseDelegate<TempoGeographic::DateTime>::CreateLambda([&Response](const TempoGeographic::DateTime& ResponseIn, grpc::Status Status)
		{
			Response = ResponseIn;
		}));
		checkf(Response.IsSet(), TEXT("GetDateTime service did not generate response immediately"));
		return Response.GetValue();
	}));
}
