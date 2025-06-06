// Copyright Tempo Simulation, LLC. All Rights Reserved.

#include "TempoScripting.h"

#include "TempoScriptingServer.h"

#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

#if WITH_EDITOR
#include "IHotReload.h"
#endif

#define LOCTEXT_NAMESPACE "FTempoScriptingModule"

DEFINE_LOG_CATEGORY(LogTempoScripting);

void FTempoScriptingModule::StartupModule()
{
	ScriptingServer = MakeUnique<FTempoScriptingServer>();

#if WITH_EDITOR
	// We disallow hot reload for TempoScripting module for the same reason we force TempoScripting
	// to re-export all the symbols from gRPC and Protobuf: these libraries have lots of static
	// global state that must exist in exactly one place in the whole executable. That doesn't play
	// well with hot reload, since it requires duplicating the TempoScripting dll, but at least some of
	// the references to the static global state remain pointing to the old dll. Yuck.
	// However, other modules that define protos can still use hot reload, and Protobuf will crash when
	// those re-generated auto-generated message classes register themselves with its global static
	// memory of all proto classes. So we reset that memory here, when a hot reload begins.
	IHotReloadModule::Get().OnModuleCompilerStarted().AddLambda([](bool)
	{
		// Note: we added these reset methods in TempoThirdParty-v0.4
		google::protobuf_tempo::DescriptorPool::ResetGeneratedDatabase();
		google::protobuf_tempo::MessageFactory::ResetGeneratedFactory();
	});
#endif
}

void FTempoScriptingModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTempoScriptingModule, TempoScripting)
