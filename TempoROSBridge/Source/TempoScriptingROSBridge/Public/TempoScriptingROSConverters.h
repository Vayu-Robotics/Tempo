// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoROSConversion.h"

#include "std_srvs/srv/empty.hpp"

#include "TempoScripting/Empty.pb.h"

template <>
struct TToROSConverter<TempoScripting::Empty> : TConverter<TToROSConverter<TempoScripting::Empty>>
{
	using ToType = std_srvs::srv::Empty::Response;
	using FromType = TempoScripting::Empty;
	static ToType Convert(const FromType& TempoValue)
	{
		return ToType();
	}
};

template <>
struct TFromROSConverter<TempoScripting::Empty> : TConverter<TFromROSConverter<TempoScripting::Empty>>
{
	using ToType = TempoScripting::Empty;
	using FromType = std_srvs::srv::Empty::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		return ToType();
	}
};

struct FTempoEmptyService
{
	using Request = TempoScripting::Empty;
	using Response = TempoScripting::Empty;
};

template <>
struct TToROSConverter<FTempoEmptyService>
{
	using FromType = FTempoEmptyService;
	using ToType = std_srvs::srv::Empty;
};
