// Copyright Tempo Simulation, LLC. All Rights Reserved.

#include "TempoAgentsEditorServiceSubsystem.h"

#include "TempoAgentsEditorUtils.h"
#include "TempoAgentsEditor/TempoAgentsEditor.grpc.pb.h"

using TempoAgentsEditorService = TempoAgentsEditor::TempoAgentsEditorService;
using TempoAgentsEditorAsyncService = TempoAgentsEditor::TempoAgentsEditorService::AsyncService;

void UTempoAgentsEditorServiceSubsystem::RegisterScriptingServices(FTempoScriptingServer& ScriptingServer)
{
	ScriptingServer.RegisterService<TempoAgentsEditorService>(
		SimpleRequestHandler(&TempoAgentsEditorAsyncService::RequestRunTempoZoneGraphBuilderPipeline, &UTempoAgentsEditorServiceSubsystem::RunTempoZoneGraphBuilderPipeline)
	);
}

void UTempoAgentsEditorServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FTempoScriptingServer::Get().ActivateService<TempoAgentsEditorService>(this);
}

void UTempoAgentsEditorServiceSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FTempoScriptingServer::Get().DeactivateService<TempoAgentsEditorService>();
}

void UTempoAgentsEditorServiceSubsystem::RunTempoZoneGraphBuilderPipeline(const TempoScripting::Empty& Request, const TResponseDelegate<TempoScripting::Empty>& ResponseContinuation) const
{
	UTempoAgentsEditorUtils::RunTempoZoneGraphBuilderPipeline();

	ResponseContinuation.ExecuteIfBound(TempoScripting::Empty(), grpc::Status_OK);
}
