// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoTimeROSBridge.h"

// #if defined(_MSC_VER)
//   #pragma optimize( "", off )
// #elif defined(__clang__)
//   #pragma clang optimize off
// #endif
// #include "tempo_time_ros_bridge/srv/detail/advance_steps__rosidl_typesupport_introspection_cpp.hpp"
// #include "tempo_time_ros_bridge/srv/detail/set_sim_steps_per_second__rosidl_typesupport_introspection_cpp.hpp"
// #include "tempo_time_ros_bridge/srv/detail/set_time_mode__rosidl_typesupport_introspection_cpp.hpp"
//
// #if defined(__GNUC__) && not defined(__clang__)
// __attribute__((optimize("O0"))
// #endif
// volatile void* tempotime_rosidl_typesupport_symbols[] = {
// 	(void*)&rosidl_typesupport_introspection_cpp__get_service_type_support_handle__tempo_time_ros_bridge__srv__AdvanceSteps,
// 	(void*)&rosidl_typesupport_introspection_cpp__get_service_type_support_handle__tempo_time_ros_bridge__srv__SetSimStepsPerSecond,
// 	(void*)&rosidl_typesupport_introspection_cpp__get_service_type_support_handle__tempo_time_ros_bridge__srv__SetTimeMode,
// };
// #if defined(_MSC_VER)
// #pragma optimize( "", on )
// #elif defined(__clang__)
// #pragma clang optimize on
// #endif

#define LOCTEXT_NAMESPACE "FTempoTimeROSBridgeModule"

void FTempoTimeROSBridgeModule::StartupModule()
{
	// auto dummy_advance_steps = rosidl_typesupport_introspection_cpp__get_service_type_support_handle__tempo_time_ros_bridge__srv__AdvanceSteps();
	// UE_LOG(LogTemp, Warning, TEXT("Identifier: %s"), UTF8_TO_TCHAR(dummy_advance_steps->typesupport_identifier));
	// auto dummy_set_sim_steps_per_second = rosidl_typesupport_introspection_cpp__get_service_type_support_handle__tempo_time_ros_bridge__srv__SetSimStepsPerSecond();
	// UE_LOG(LogTemp, Warning, TEXT("Identifier: %s"), UTF8_TO_TCHAR(dummy_set_sim_steps_per_second->typesupport_identifier));
	// auto dummy_time_mode = rosidl_typesupport_introspection_cpp__get_service_type_support_handle__tempo_time_ros_bridge__srv__SetTimeMode();
	// UE_LOG(LogTemp, Warning, TEXT("Identifier: %s"), UTF8_TO_TCHAR(dummy_time_mode->typesupport_identifier));
}

void FTempoTimeROSBridgeModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FTempoTimeROSBridgeModule, TempoTimeROSBridge)
