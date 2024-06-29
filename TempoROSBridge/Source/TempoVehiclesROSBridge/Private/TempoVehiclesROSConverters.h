// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoROSConversion.h"

#include "tempo_vehicles_ros_bridge/srv/get_commandable_vehicles.hpp"
#include "tempo_vehicles_ros_bridge/srv/command_vehicle.hpp"

#include "TempoVehicles/Driving.grpc.pb.h"

template <>
struct TFromROSConverter<TempoScripting::Empty> : TConverter<TFromROSConverter<TempoScripting::Empty>>
{
	using ToType = TempoScripting::Empty;
	using FromType = tempo_vehicles_ros_bridge::srv::GetCommandableVehicles::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		return ToType();
	}
};


template <>
struct TToROSConverter<TempoVehicles::CommandableVehiclesResponse> : TConverter<TFromROSConverter<TempoVehicles::CommandableVehiclesResponse>>
{
	using ToType = tempo_vehicles_ros_bridge::srv::GetCommandableVehicles::Response;
	using FromType = TempoVehicles::CommandableVehiclesResponse;
	static ToType Convert(const FromType& TempoValue)
	{
		ToType ROSValue;
		for (const std::string& vehicle_name : TempoValue.vehicle_name())
		{
			ROSValue.vehicle_names.append(vehicle_name);
		}
		return ROSValue;
	}
};

template <>
struct TFromROSConverter<TempoVehicles::DrivingCommandRequest> : TConverter<TFromROSConverter<TempoVehicles::DrivingCommandRequest>>
{
	using ToType = TempoVehicles::DrivingCommandRequest;
	using FromType = tempo_vehicles_ros_bridge::srv::CommandVehicle::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		ToType TempoValue;
		TempoValue.set_vehicle_name(ROSValue.vehicle_name);
		TempoValue.set_acceleration(ROSValue.acceleration);
		TempoValue.set_steering(ROSValue.steering);
		return TempoValue;
	}
};

struct FTempoGetCommandableVehiclesService
{
	using Request = tempo_vehicles_ros_bridge::srv::GetCommandableVehicles::Request;
	using Response = TempoVehicles::CommandableVehiclesResponse;
};

template <>
struct TToROSConverter<FTempoGetCommandableVehiclesService>
{
	using FromType = FTempoGetCommandableVehiclesService;
	using ToType = tempo_vehicles_ros_bridge::srv::GetCommandableVehicles;
};

struct FTempoCommandVehicleService
{
	using Request = TempoVehicles::DrivingCommandRequest;
	using Response = tempo_vehicles_ros_bridge::srv::CommandVehicle::Response;
};

template <>
struct TToROSConverter<FTempoCommandVehicleService>
{
	using FromType = FTempoCommandVehicleService;
	using ToType = tempo_vehicles_ros_bridge::srv::CommandVehicle;
};
