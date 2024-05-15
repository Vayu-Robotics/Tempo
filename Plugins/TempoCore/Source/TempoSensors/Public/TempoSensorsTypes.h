// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TempoSensorsTypes.generated.h"

UENUM(BlueprintType)
enum EImageType: uint8
{
	RGB = 0,
	DEPTH = 1,
	SEMANTIC_LABELS = 2,
};
