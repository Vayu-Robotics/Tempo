// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoSensorsTypes.h"

#include "CoreMinimal.h"
#include "TempoSensorServiceSubsystem.h"
#include "TempoSensorsROSBridgeSubsystem.generated.h"

UCLASS()
class TEMPOSENSORSROSBRIDGE_API UTempoSensorsROSBridgeSubsystem : public UTempoSensorServiceSubsystem
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

protected:
	void UpdatePublishers();

	virtual void AddImagePublisher(const EMeasurementType& MeasurementType, FString Topic);

	virtual void PublishColorImage(const TempoCamera::ColorImage& Image, FString Topic);

	virtual void PublishLabelImage(const TempoCamera::LabelImage& Image, FString Topic);

	virtual void PublishDepthImage(const TempoCamera::DepthImage& Image, FString Topic);

	template <typename MeasurementType>
	void PublishImage(const MeasurementType& Image, FString Topic);

	template <typename MeasurementType>
	void OnMeasurementReceived(const MeasurementType& Image, grpc::Status Status, FString Topic);

	template <typename MeasurementType>
	void RequestMeasurement(const FString& Topic, const TResponseDelegate<MeasurementType>& ResponseContinuation);

	FTimerHandle UpdatePublishersTimerHandle;

	float UpdatePublishersPeriod = 1.0;

	FCriticalSection MeasurementReceivedMutex;
	
	TSet<FString> TopicsWithPendingRequests;
	
	UPROPERTY()
	class UTempoROSNode* ROSNode;
};
