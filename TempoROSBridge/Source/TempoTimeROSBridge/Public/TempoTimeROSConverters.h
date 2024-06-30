// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoROSConversion.h"

#include "tempo_time_ros_bridge/srv/advance_steps.hpp"
#include "tempo_time_ros_bridge/srv/set_sim_steps_per_second.hpp"
#include "tempo_time_ros_bridge/srv/set_time_mode.hpp"

#include "TempoTime/Time.grpc.pb.h"

template <>
struct TFromROSConverter<TempoTime::AdvanceStepsRequest> : TConverter<TFromROSConverter<TempoTime::AdvanceStepsRequest>>
{
	using ToType = TempoTime::AdvanceStepsRequest;
	using FromType = tempo_time_ros_bridge::srv::AdvanceSteps::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		ToType TempoValue;
		TempoValue.set_steps(ROSValue.steps);
		return TempoValue;
	}
};

template <>
struct TFromROSConverter<TempoTime::SetSimStepsPerSecondRequest> : TConverter<TFromROSConverter<TempoTime::SetSimStepsPerSecondRequest>>
{
	using ToType = TempoTime::SetSimStepsPerSecondRequest;
	using FromType = tempo_time_ros_bridge::srv::SetSimStepsPerSecond::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		ToType TempoValue;
		TempoValue.set_sim_steps_per_second(ROSValue.sim_steps_per_second);
		return TempoValue;
	}
};

template <>
struct TFromROSConverter<TempoTime::TimeModeRequest> : TConverter<TFromROSConverter<TempoTime::TimeModeRequest>>
{
	using ToType = TempoTime::TimeModeRequest;
	using FromType = tempo_time_ros_bridge::srv::SetTimeMode::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		ToType TempoValue;
		if (ROSValue.time_mode == "fixed_step")
		{
			TempoValue.set_time_mode(TempoTime::FIXED_STEP);
		}
		else if (ROSValue.time_mode == "wall_clock")
		{
			TempoValue.set_time_mode(TempoTime::WALL_CLOCK);
		}
		else
		{
			checkf(false, TEXT("Unhandled time mode"));
		}
		return TempoValue;
	}
};

struct FTempoAdvanceStepsService
{
	using Request = TempoTime::AdvanceStepsRequest;
	using Response = tempo_time_ros_bridge::srv::AdvanceSteps::Response;
};

template <>
struct TToROSConverter<FTempoAdvanceStepsService>
{
	using FromType = FTempoAdvanceStepsService;
	using ToType = tempo_time_ros_bridge::srv::AdvanceSteps;
};

struct FTempoSetSimStepsPerSecondService
{
	using Request = TempoTime::SetSimStepsPerSecondRequest;
	using Response = tempo_time_ros_bridge::srv::SetSimStepsPerSecond::Response;
};

template <>
struct TToROSConverter<FTempoSetSimStepsPerSecondService>
{
	using FromType = FTempoSetSimStepsPerSecondService;
	using ToType = tempo_time_ros_bridge::srv::SetSimStepsPerSecond;
};

struct FTempoSetTimeModeService
{
	using Request = TempoTime::TimeModeRequest;
	using Response = tempo_time_ros_bridge::srv::SetTimeMode::Response;
};

template <>
struct TToROSConverter<FTempoSetTimeModeService>
{
	using FromType = FTempoSetTimeModeService;
	using ToType = tempo_time_ros_bridge::srv::SetTimeMode;
};
