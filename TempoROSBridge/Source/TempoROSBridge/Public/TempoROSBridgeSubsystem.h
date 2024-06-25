// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "sensor_msgs/msg/image.hpp"
#include "std_srvs/srv/empty.hpp"

#include "TempoROSService.h"

#include "TempoROSCommonConverters.h"

#include "TempoScripting/Empty.pb.h"

#include "TempoCamera/Camera.pb.h"
#include "TempoScriptingServer.h"

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TempoROSBridgeSubsystem.generated.h"

template <>
struct TToROSConverter<TempoCamera::ColorImage>
{
	using ToType = sensor_msgs::msg::Image;
	using FromType = TempoCamera::ColorImage;
	static ToType Convert(const FromType& FromValue)
	{
		return ToType();
	}
};

template <>
struct TToROSConverter<TempoScripting::Empty>
{
	using ToType = std_srvs::srv::Empty::Response;
	using FromType = TempoScripting::Empty;
	static ToType Convert(const FromType& From)
	{
		return ToType();
	}
};

template <>
struct TFromROSConverter<TempoScripting::Empty>
{
	using FromType = std_srvs::srv::Empty::Request;
	using ToType = TempoScripting::Empty;
	static ToType Convert(const FromType& From)
	{
		return ToType();
	}
};

struct TempoEmptyService
{
	using Request = TempoScripting::Empty;
	using Response = TempoScripting::Empty;
};

template <>
struct TToROSConverter<TempoEmptyService>
{
	using ToType = std_srvs::srv::Empty;
	using FromType = TempoEmptyService;
};

/**
 * 
 */
UCLASS()
class TEMPOROSBRIDGE_API UTempoROSBridgeSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	void OnWorldBeginPlay(UWorld& InWorld) override;

	ENGINE_API virtual void Tick(float DeltaTime) override;
	ENGINE_API virtual TStatId GetStatId() const override;
	
protected:
	void UpdatePublications();
	
	UPROPERTY(EditAnywhere)
	float UpdatePublicationsPeriod = 1.0;
	
private:
	void OnCameraImageReceived(const TempoCamera::ColorImage& Image, grpc::Status, FString CameraName);
	
	FTimerHandle UpdatePublicationsTimerHandle;

	// TMap<FString, TTempoROSPublisher<TempoCamera::ColorImage>> Publishers;
	// TOptional<TTempoROSPublisher<FString>> MsgPublisher;
	// TOptional<TTempoROSSubscription<FString>> Subscription;
	TOptional<TTempoROSService<TempoEmptyService>> PlayPauseService;

	UPROPERTY()
	class UTempoROSNode* ROSNode;
};
