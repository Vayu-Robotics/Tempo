// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoROSBridgeSubsystem.h"

#include "TempoROSNode.h"

#include "TempoSensorServiceSubsystem.h"
#include "TempoSensors/Sensors.grpc.pb.h"

#include "tempo_ros_bridge/msg/test_one.hpp"
#include "tempo_ros_bridge/srv/test_service.hpp"

void UTempoROSBridgeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	if (InWorld.WorldType != EWorldType::Game && InWorld.WorldType != EWorldType::PIE)
	{
		return;
	}

	tempo_ros_bridge::msg::TestOne test;
	tempo_ros_bridge::srv::TestService test_service;

	ROSNode = NewObject<UTempoROSNode>(this);

	InWorld.GetTimerManager().SetTimer(UpdatePublicationsTimerHandle, this, &UTempoROSBridgeSubsystem::UpdatePublications, UpdatePublicationsPeriod, true);
	ROSNode->AddPublisher<FString>("TempoROSBridgeStatus");
	ROSNode->AddSubscription("TempoROSBridgeStatus", TROSSubscriptionDelegate<FString>::CreateLambda([](const FString& Message)
	{
		UE_LOG(LogTemp, Warning, TEXT("Received %s"), *Message);
	}));
	ROSNode->AddService<TempoEmptyService>("PlayPause", TROSServiceDelegate<TempoEmptyService>::CreateLambda([](const TempoScripting::Empty& Request)
	{
		UE_LOG(LogTemp, Warning, TEXT("Play/Pause"));
		return TempoScripting::Empty();
	}));
}

void UTempoROSBridgeSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ROSNode->Tick();
}

void UTempoROSBridgeSubsystem::UpdatePublications()
{
	std_msgs::msg::String status;
	const FString Message = FString::Printf(TEXT("Alive at %f"), GetWorld()->GetTimeSeconds());
	ROSNode->Publish("TempoROSBridgeStatus", Message);
	
	UE_LOG(LogTemp, Warning, TEXT("UpdatePublications"));
	const TempoSensors::AvailableSensorsRequest Request;
	TArray<FString> AvailableSensorNames;
	GetWorld()->GetSubsystem<UTempoSensorServiceSubsystem>()->GetAvailableSensors(Request,  TResponseDelegate<TempoSensors::AvailableSensorsResponse>::CreateLambda([&AvailableSensorNames](const TempoSensors::AvailableSensorsResponse& Response, grpc::Status)
	{
		for (const TempoSensors::SensorDescriptor& AvailableSensor : Response.available_sensors())
		{
			AvailableSensorNames.Add(UTF8_TO_TCHAR(AvailableSensor.name().c_str()));
		}
	}));
	
	for (const FString& SensorName : AvailableSensorNames)
	{
		// if (!Publishers.Contains(SensorName))
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Adding publisher for %s"), *SensorName);
		// 	// Publishers.Add(SensorName, ROSNode->AddPublisher<TempoCamera::ColorImage>(TCHAR_TO_UTF8(*SensorName)));
		// }
		TempoCamera::ColorImageRequest ImageRequest;
		ImageRequest.set_sensor_name(TCHAR_TO_UTF8(*SensorName));
		GetWorld()->GetSubsystem<UTempoSensorServiceSubsystem>()->StreamColorImages(ImageRequest, TResponseDelegate<TempoCamera::ColorImage>::CreateUObject(this, &UTempoROSBridgeSubsystem::OnCameraImageReceived, SensorName));
	}
}

void UTempoROSBridgeSubsystem::OnCameraImageReceived(const TempoCamera::ColorImage& Image, grpc::Status, FString CameraName)
{
	// sensor_msgs::msg::Image ROSImage;
	// ROSImage.height = Image.height();
	// ROSImage.width = Image.width();
	// ROSImage.data = std::vector<uint8>(Image.data().begin(), Image.data().end());
	// Publishers[CameraName].Publish(Image);
	TempoCamera::ColorImageRequest ImageRequest;
	ImageRequest.set_sensor_name(TCHAR_TO_UTF8(*CameraName));
	GetWorld()->GetSubsystem<UTempoSensorServiceSubsystem>()->StreamColorImages(ImageRequest, TResponseDelegate<TempoCamera::ColorImage>::CreateUObject(this, &UTempoROSBridgeSubsystem::OnCameraImageReceived, CameraName));
}

TStatId UTempoROSBridgeSubsystem::GetStatId() const
{
	return TStatId();
}

