// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TempoSensorsTypes.generated.h"

UENUM(BlueprintType)
enum EImageType: uint8
{
	COLOR = 0,
	DEPTH_AND_LABELS = 1,
};

USTRUCT(BlueprintInternalUseOnly)
struct FSemanticLabel: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LabelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Label;
};
