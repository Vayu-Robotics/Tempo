// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoROSConversion.h"

#include "tempo_sensors_ros_bridge/srv/get_available_sensors.hpp"
#include "sensor_msgs/msg/image.hpp"

#include "TempoSensors/Sensors.grpc.pb.h"
#include "TempoCamera/Camera.pb.h"

DEFINE_TEMPOROS_MESSAGE_TYPE_TRAITS(TempoCamera::ColorImage);
DEFINE_TEMPOROS_MESSAGE_TYPE_TRAITS(TempoCamera::DepthImage);
DEFINE_TEMPOROS_MESSAGE_TYPE_TRAITS(TempoCamera::LabelImage);

template <>
struct TFromROSConverter<tempo_sensors_ros_bridge::srv::GetAvailableSensors::Request, TempoSensors::AvailableSensorsRequest>
{
	static TempoSensors::AvailableSensorsRequest Convert(const tempo_sensors_ros_bridge::srv::GetAvailableSensors::Request& ROSValue)
	{
		return TempoSensors::AvailableSensorsRequest();
	}
};

template <>
struct TToROSConverter<tempo_sensors_ros_bridge::srv::GetAvailableSensors::Response, TempoSensors::AvailableSensorsResponse>
{
	static tempo_sensors_ros_bridge::srv::GetAvailableSensors::Response Convert(const TempoSensors::AvailableSensorsResponse& TempoValue)
	{
		tempo_sensors_ros_bridge::srv::GetAvailableSensors::Response ROSType;
		ROSType.available_sensors.resize(TempoValue.available_sensors().size());
		for (const auto& TempoAvailableSensor : TempoValue.available_sensors())
		{
			tempo_sensors_ros_bridge::msg::SensorDescriptor ROSAvailableSensor;
			ROSAvailableSensor.name = TempoAvailableSensor.name();
			ROSAvailableSensor.owner = TempoAvailableSensor.owner();
			ROSAvailableSensor.rate = TempoAvailableSensor.rate();
			for (const auto& MeasurementType : TempoAvailableSensor.measurement_types())
			{
				switch (MeasurementType)
				{
					case TempoSensors::COLOR_IMAGE:
					{
						ROSAvailableSensor.measurement_types.push_back("color_image");
						break;
					}
					case TempoSensors::DEPTH_IMAGE:
					{
						ROSAvailableSensor.measurement_types.push_back("depth_image");
						break;
					}
					case TempoSensors::LABEL_IMAGE:
					{
						ROSAvailableSensor.measurement_types.push_back("label_image");
						break;
					}
					default:
					{
						checkf(false, TEXT("Unhandled measurement type"));
					}
				}
			}
			ROSType.available_sensors.push_back(ROSAvailableSensor);
		}
		return ROSType;
	}
};

struct FTempoGetAvailableSensors
{
	using Request = TempoSensors::AvailableSensorsRequest;
	using Response = TempoSensors::AvailableSensorsResponse;
};

template <>
struct TImplicitToROSConverter<FTempoGetAvailableSensors>
{
	using ToType = tempo_sensors_ros_bridge::srv::GetAvailableSensors;
	using FromType = FTempoGetAvailableSensors;
};

template <>
struct TImplicitToROSConverter<TempoCamera::ColorImage>: TToROSConverter<sensor_msgs::msg::Image, TempoCamera::ColorImage>
{
	static ToType Convert(const FromType& TempoValue)
	{
		ToType ToValue;
		ToValue.encoding = "bgra8";
		ToValue.data.assign(TempoValue.data().begin(), TempoValue.data().end());
		ToValue.width = TempoValue.width();
		ToValue.height = TempoValue.height();
		return ToValue;
	}
};

template <>
struct TImplicitToROSConverter<TempoCamera::DepthImage>: TToROSConverter<sensor_msgs::msg::Image, TempoCamera::DepthImage>
{
	static ToType Convert(const FromType& TempoValue)
	{
		ToType ToValue;
		ToValue.encoding = "32FC1";
		ToValue.data.assign(TempoValue.depths().begin(), TempoValue.depths().end());
		ToValue.width = TempoValue.width();
		ToValue.height = TempoValue.height();
		return ToValue;
	}
};

template <>
struct TImplicitToROSConverter<TempoCamera::LabelImage>: TToROSConverter<sensor_msgs::msg::Image, TempoCamera::LabelImage>
{
	static ToType Convert(const FromType& TempoValue)
	{
		ToType ToValue;
		ToValue.encoding = "mono8";
		ToValue.data.assign(TempoValue.data().begin(), TempoValue.data().end());
		ToValue.width = TempoValue.width();
		ToValue.height = TempoValue.height();
		return ToValue;
	}
};
