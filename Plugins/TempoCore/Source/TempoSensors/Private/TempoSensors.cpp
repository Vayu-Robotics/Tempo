// Copyright Tempo Simulation, LLC. All Rights Reserved.

#include "TempoSensors.h"

#define LOCTEXT_NAMESPACE "FTempoSensorsModule"

DEFINE_LOG_CATEGORY(LogTempoSensors);

void FTempoSensorsModule::StartupModule()
{
}

void FTempoSensorsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTempoSensorsModule, TempoSensors)
