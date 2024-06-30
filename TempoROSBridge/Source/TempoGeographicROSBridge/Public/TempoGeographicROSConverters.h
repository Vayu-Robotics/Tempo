// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoROSConversion.h"

#include "tempo_geographic_ros_bridge/srv/get_date_time.hpp"
#include "tempo_geographic_ros_bridge/srv/set_date.hpp"
#include "tempo_geographic_ros_bridge/srv/set_day_cycle_relative_rate.hpp"
#include "tempo_geographic_ros_bridge/srv/set_geographic_reference.hpp"
#include "tempo_geographic_ros_bridge/srv/set_time_of_day.hpp"

#include "TempoGeographic/Geographic.grpc.pb.h"

template <>
struct TFromROSConverter<TempoGeographic::Date> : TConverter<TFromROSConverter<TempoGeographic::Date>>
{
	using ToType = TempoGeographic::Date;
	using FromType = tempo_geographic_ros_bridge::srv::SetDate::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		ToType TempoValue;
		TempoValue.set_day(ROSValue.day);
		TempoValue.set_month(ROSValue.month);
		TempoValue.set_year(ROSValue.year);
		return TempoValue;
	}
};

struct FTempoSetDateService
{
	using Request = TempoGeographic::Date;
	using Response = tempo_geographic_ros_bridge::srv::SetDate::Response;
};

template <>
struct TToROSConverter<FTempoSetDateService> : TConverter<TToROSConverter<FTempoSetDateService>>
{
	using ToType = tempo_geographic_ros_bridge::srv::SetDate;
	using FromType = FTempoSetDateService;
};

template <>
struct TFromROSConverter<TempoGeographic::TimeOfDay> : TConverter<TFromROSConverter<TempoGeographic::TimeOfDay>>
{
	using ToType = TempoGeographic::TimeOfDay;
	using FromType = tempo_geographic_ros_bridge::srv::SetTimeOfDay::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		ToType TempoValue;
		TempoValue.set_hour(ROSValue.hour);
		TempoValue.set_minute(ROSValue.minute);
		TempoValue.set_second(ROSValue.second);
		return TempoValue;
	}
};

struct FTempoSetTimeOfDayService
{
	using Request = TempoGeographic::TimeOfDay;
	using Response = tempo_geographic_ros_bridge::srv::SetTimeOfDay::Response;
};

template <>
struct TToROSConverter<FTempoSetTimeOfDayService> : TConverter<TToROSConverter<FTempoSetTimeOfDayService>>
{
	using ToType = tempo_geographic_ros_bridge::srv::SetTimeOfDay;
	using FromType = FTempoSetTimeOfDayService;
};

template <>
struct TFromROSConverter<TempoGeographic::GeographicCoordinate> : TConverter<TFromROSConverter<TempoGeographic::GeographicCoordinate>>
{
	using ToType = TempoGeographic::GeographicCoordinate;
	using FromType = tempo_geographic_ros_bridge::srv::SetGeographicReference::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		ToType TempoValue;
		TempoValue.set_latitude(ROSValue.latitude);
		TempoValue.set_longitude(ROSValue.longitude);
		TempoValue.set_altitude(ROSValue.altitude);
		return TempoValue;
	}
};

struct FTempoSetGeographicReferenceService
{
	using Request = TempoGeographic::GeographicCoordinate;
	using Response = tempo_geographic_ros_bridge::srv::SetGeographicReference::Response;
};

template <>
struct TToROSConverter<FTempoSetGeographicReferenceService> : TConverter<TToROSConverter<FTempoSetGeographicReferenceService>>
{
	using ToType = tempo_geographic_ros_bridge::srv::SetGeographicReference;
	using FromType = FTempoSetGeographicReferenceService;
};

template <>
struct TFromROSConverter<TempoGeographic::DayCycleRateRequest> : TConverter<TFromROSConverter<TempoGeographic::DayCycleRateRequest>>
{
	using ToType = TempoGeographic::DayCycleRateRequest;
	using FromType = tempo_geographic_ros_bridge::srv::SetDayCycleRelativeRate::Request;
	static ToType Convert(const FromType& ROSValue)
	{
		ToType TempoValue;
		TempoValue.set_rate(ROSValue.rate);
		return TempoValue;
	}
};

struct FTempoSetDayCycleRateService
{
	using Request = TempoGeographic::DayCycleRateRequest;
	using Response = tempo_geographic_ros_bridge::srv::SetDayCycleRelativeRate::Response;
};

template <>
struct TToROSConverter<FTempoSetDayCycleRateService> : TConverter<TToROSConverter<FTempoSetDayCycleRateService>>
{
	using ToType = tempo_geographic_ros_bridge::srv::SetDayCycleRelativeRate;
	using FromType = FTempoSetDayCycleRateService;
};

template <>
struct TToROSConverter<TempoGeographic::DateTime> : TConverter<TToROSConverter<TempoGeographic::DateTime>>
{
	using ToType = tempo_geographic_ros_bridge::srv::GetDateTime::Response;
	using FromType = TempoGeographic::DateTime;
	static ToType Convert(const FromType& TempoValue)
	{
		ToType ROSValue;
		ROSValue.day = TempoValue.date().day();
		ROSValue.month = TempoValue.date().month();
		ROSValue.year = TempoValue.date().year();
		ROSValue.hour = TempoValue.time().hour();
		ROSValue.minute = TempoValue.time().minute();
		ROSValue.second = TempoValue.time().second();
		return ROSValue;
	}
};

struct FTempoGetDateTimeService
{
	using Request = tempo_geographic_ros_bridge::srv::GetDateTime::Request;
	using Response = TempoGeographic::DateTime;
};

template <>
struct TToROSConverter<FTempoGetDateTimeService> : TConverter<TToROSConverter<FTempoGetDateTimeService>>
{
	using ToType = tempo_geographic_ros_bridge::srv::GetDateTime;
	using FromType = FTempoGetDateTimeService;
};
