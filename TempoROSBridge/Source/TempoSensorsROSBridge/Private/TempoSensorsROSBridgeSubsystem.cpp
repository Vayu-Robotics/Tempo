// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoSensorsROSBridgeSubsystem.h"

#include "TempoSensorsROSConverters.h"

#include "TempoROSNode.h"

#include "TempoROSBridgeUtils.h"

FString MeasurementTypeStr(TempoSensors::MeasurementType MeasurementType)
{
	switch (MeasurementType)
	{
	case TempoSensors::COLOR_IMAGE:
		{
			return TEXT("image/color");
		}
	case TempoSensors::DEPTH_IMAGE:
		{
			return TEXT("image/depth");
		}
	case TempoSensors::LABEL_IMAGE:
		{
			return TEXT("image/label");
		}
	default:
		{
			checkf(false, TEXT("Unhandled measurement type"));
			return TEXT("");
		}
	}
}

FString TopicFromSensorInfo(TempoSensors::MeasurementType MeasurementType, const FString& OwnerName, const FString& SensorName)
{
	return FString::Printf(TEXT("%s/%s/%s"), *MeasurementTypeStr(MeasurementType), *OwnerName.ToLower(), *SensorName.ToLower());
}

FString SensorNameFromTopic(const FString& Topic)
{
	TArray<FString> SplitTopic;
	Topic.ParseIntoArray(SplitTopic, TEXT("/"));
	return SplitTopic.Last();
}

FString OwnerNameFromTopic(const FString& Topic)
{
	TArray<FString> SplitTopic;
	Topic.ParseIntoArray(SplitTopic, TEXT("/"));
	return SplitTopic[SplitTopic.Num() - 2];
}

FString MeasurementTypeFromTopic(const FString& Topic)
{
	TArray<FString> SplitTopic;
	Topic.ParseIntoArray(SplitTopic, TEXT("/"));
	return SplitTopic[SplitTopic.Num() - 3];
}

void UTempoSensorsROSBridgeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	if (InWorld.WorldType != EWorldType::Game && InWorld.WorldType != EWorldType::PIE)
	{
		return;
	}

	ROSNode = UTempoROSNode::Create("TempoSensors", this, &InWorld);
	BindScriptingServiceToROS<FTempoGetAvailableSensors, UTempoSensorServiceSubsystem>(ROSNode, "GetAvailableSensors", this, &UTempoSensorsROSBridgeSubsystem::GetAvailableSensors);

	InWorld.GetTimerManager().SetTimer(UpdatePublishersTimerHandle, this, &UTempoSensorsROSBridgeSubsystem::UpdatePublishers, UpdatePublishersPeriod, true, 0.0);
}

template <class...>
struct False : std::bool_constant<false> { };

template <typename MeasurementType>
void UTempoSensorsROSBridgeSubsystem::RequestMeasurement(const FString& Topic, const TResponseDelegate<MeasurementType>& ResponseContinuation)
{
	static_assert(False<MeasurementType>{}, "RequestMeasurement called with unsupported image type");
}

template <>
void UTempoSensorsROSBridgeSubsystem::RequestMeasurement<TempoCamera::ColorImage>(const FString& Topic, const TResponseDelegate<TempoCamera::ColorImage>& ResponseContinuation)
{
	TempoCamera::ColorImageRequest ImageRequest;
	const FString SensorName = SensorNameFromTopic(Topic);
	const FString OwnerName = OwnerNameFromTopic(Topic);
	ImageRequest.set_sensor_name(TCHAR_TO_UTF8(*SensorName));
	ImageRequest.set_owner_name(TCHAR_TO_UTF8(*OwnerName));
	StreamColorImages(
		ImageRequest,
		TResponseDelegate<TempoCamera::ColorImage>::CreateUObject(
			this, &UTempoSensorsROSBridgeSubsystem::OnMeasurementReceived, Topic));
}

template <>
void UTempoSensorsROSBridgeSubsystem::RequestMeasurement<TempoCamera::DepthImage>(const FString& Topic, const TResponseDelegate<TempoCamera::DepthImage>& ResponseContinuation)
{
	TempoCamera::DepthImageRequest ImageRequest;
	const FString SensorName = SensorNameFromTopic(Topic);
	const FString OwnerName = OwnerNameFromTopic(Topic);
	ImageRequest.set_sensor_name(TCHAR_TO_UTF8(*SensorName));
	ImageRequest.set_owner_name(TCHAR_TO_UTF8(*OwnerName));
	StreamDepthImages(
		ImageRequest,
		TResponseDelegate<TempoCamera::DepthImage>::CreateUObject(
			this, &UTempoSensorsROSBridgeSubsystem::OnMeasurementReceived, Topic));
}

template <>
void UTempoSensorsROSBridgeSubsystem::RequestMeasurement<TempoCamera::LabelImage>(const FString& Topic, const TResponseDelegate<TempoCamera::LabelImage>& ResponseContinuation)
{
	TempoCamera::LabelImageRequest ImageRequest;
	const FString SensorName = SensorNameFromTopic(Topic);
	const FString OwnerName = OwnerNameFromTopic(Topic);
	ImageRequest.set_sensor_name(TCHAR_TO_UTF8(*SensorName));
	ImageRequest.set_owner_name(TCHAR_TO_UTF8(*OwnerName));
	StreamLabelImages(
		ImageRequest,
		TResponseDelegate<TempoCamera::LabelImage>::CreateUObject(
			this, &UTempoSensorsROSBridgeSubsystem::OnMeasurementReceived, Topic));
}

void UTempoSensorsROSBridgeSubsystem::UpdatePublishers()
{
	const TempoSensors::AvailableSensorsRequest Request;
	TSet<FString> PossiblyStaleTopics = ROSNode->GetPublishedTopics();
	GetAvailableSensors(
		Request,
		TResponseDelegate<TempoSensors::AvailableSensorsResponse>::CreateLambda(
			[this, &PossiblyStaleTopics](const TempoSensors::AvailableSensorsResponse& Response, grpc::Status)
	{
		for (const TempoSensors::SensorDescriptor& AvailableSensor : Response.available_sensors())
		{
			for (const auto MeasurementType : AvailableSensor.measurement_types())
			{
				const FString Topic = TopicFromSensorInfo(
					static_cast<TempoSensors::MeasurementType>(MeasurementType),
					AvailableSensor.owner().c_str(),
					AvailableSensor.name().c_str());

				bool bAlreadyHasTopic = PossiblyStaleTopics.Contains(Topic);
				PossiblyStaleTopics.Remove(Topic);
				if (bAlreadyHasTopic)
				{
					// We already have a publisher for this topic
					continue;
				}
				
				switch (MeasurementType)
				{
					case TempoSensors::COLOR_IMAGE:
						{
							ROSNode->AddPublisher<TempoCamera::ColorImage>(Topic);
							break;
						}
					case TempoSensors::DEPTH_IMAGE:
						{
							ROSNode->AddPublisher<TempoCamera::DepthImage>(Topic);
							break;
						}
					case TempoSensors::LABEL_IMAGE:
						{
							ROSNode->AddPublisher<TempoCamera::LabelImage>(Topic);
							break;
						}
					default:
						{
							checkf(false, TEXT("Unhandled measurement type"));
						}
				}
			}
		}
	}));

	for (const FString& StaleTopic : PossiblyStaleTopics)
	{
		ROSNode->RemovePublisher(StaleTopic);
	}

	const TMap<FString, FTempoROSPublisher>& Publishers = ROSNode->GetPublishers();
	for (const auto& Elem : Publishers)
	{
		const FString& Topic = Elem.Key;
		const FTempoROSPublisher& Publisher = Elem.Value;
		if (Publisher.HasSubscriptions() && !TopicsWithPendingRequests.Contains(Topic))
		{
			const FString MeasurementType = MeasurementTypeFromTopic(Topic);
			if (MeasurementType == TEXT("color"))
			{
				RequestMeasurement<TempoCamera::ColorImage>(
					Topic,
					TResponseDelegate<TempoCamera::ColorImage>::CreateUObject(
						this, &UTempoSensorsROSBridgeSubsystem::OnMeasurementReceived, Topic));
			}
			else if (MeasurementType == TEXT("depth"))
			{
				RequestMeasurement<TempoCamera::DepthImage>(
					Topic,
					TResponseDelegate<TempoCamera::DepthImage>::CreateUObject(
						this, &UTempoSensorsROSBridgeSubsystem::OnMeasurementReceived, Topic));
			}
			else if (MeasurementType == TEXT("label"))
			{
				RequestMeasurement<TempoCamera::LabelImage>(
					Topic,
					TResponseDelegate<TempoCamera::LabelImage>::CreateUObject(
						this, &UTempoSensorsROSBridgeSubsystem::OnMeasurementReceived, Topic));
			}

			TopicsWithPendingRequests.Add(Topic);
		}
	}
}

template <typename MeasurementType>
void UTempoSensorsROSBridgeSubsystem::OnMeasurementReceived(const MeasurementType& Image, grpc::Status Status, FString Topic)
{
	if (Status.ok())
	{
		ROSNode->Publish(Topic, Image);
	}
	
	TopicsWithPendingRequests.Remove(Topic);

	const TMap<FString, FTempoROSPublisher>& Publishers = ROSNode->GetPublishers();
	if (const FTempoROSPublisher* Publisher = Publishers.Find(Topic); Publisher->HasSubscriptions())
	{
		RequestMeasurement<MeasurementType>(
			Topic,
			TResponseDelegate<MeasurementType>::CreateUObject(
				this, &UTempoSensorsROSBridgeSubsystem::OnMeasurementReceived, Topic));
		TopicsWithPendingRequests.Add(Topic);
	}
}