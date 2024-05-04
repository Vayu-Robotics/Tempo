// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "StreamedCamera.h"

#include "TempoScriptingServer.h"

#include "Tempo/Camera.grpc.pb.h"
#include "TempoScripting/Empty.pb.h"

using CameraService = Tempo::CameraService::AsyncService;
using TempoEmpty = TempoScripting::Empty;

UStreamedCamera::UStreamedCamera()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStreamedCamera::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UStreamedCamera::SendImage, 1.0, true);
}

void UStreamedCamera::RegisterWorldServices(UTempoScriptingServer* ScriptingServer)
{
	UE_LOG(LogTemp, Warning, TEXT("RegisterWorldServices %d"), GetWorld()->WorldType);
	ScriptingServer->RegisterService<CameraService>(
		TStreamRequestHandler<CameraService, TempoEmpty, TempoEmpty>(&CameraService::RequestStreamImages).BindUObject(this, &UStreamedCamera::StreamCameraImages)
		);
}

void UStreamedCamera::StreamCameraImages(const TempoEmpty& Request, const ResponseContinuationType<TempoEmpty>& ResponseContinuation)
{
	PendingResponseContinuation = ResponseContinuation;
}

void UStreamedCamera::SendImage()
{
	UE_LOG(LogTemp, Warning, TEXT("SendImage"));
	
	if (PendingResponseContinuation.IsSet())
	{
		PendingResponseContinuation.GetValue()(TempoEmpty(), grpc::Status());
	}
}
