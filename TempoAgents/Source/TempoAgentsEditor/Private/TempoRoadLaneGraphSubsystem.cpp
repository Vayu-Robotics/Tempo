// Copyright Tempo Simulation, LLC. All Rights Reserved


#include "TempoRoadLaneGraphSubsystem.h"

#include "EngineUtils.h"
#include "TempoAgentsEditor.h"
#include "TempoCrosswalkInterface.h"
#include "TempoRoadInterface.h"
#include "TempoIntersectionInterface.h"
#include "ZoneGraphSettings.h"
#include "ZoneShapeComponent.h"
#include "ZoneGraphSubsystem.h"
#include "ZoneGraphDelegates.h"
#include "DrawDebugHelpers.h"
#include "TempoRoadModuleInterface.h"
#include "VectorTypes.h"

//
// Blueprint interface functions
//

void UTempoRoadLaneGraphSubsystem::SetupZoneGraphBuilder()
{
	UZoneGraphSubsystem* ZoneGraphSubsystem = UWorld::GetSubsystem<UZoneGraphSubsystem>(GetWorld());

	if (ensureMsgf(ZoneGraphSubsystem != nullptr, TEXT("Can't access UZoneGraphSubsystem during call to SetupZoneGraphBuilder.")))
	{
		ZoneGraphSubsystem->RegisterBuilder(&TempoZoneGraphBuilder);
	}
}

bool UTempoRoadLaneGraphSubsystem::TryGenerateZoneShapeComponents() const
{
	IConsoleVariable* RemoveOverlapCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ai.debug.zonegraph.generation.RemoveOverlap"));
	if (ensureMsgf(RemoveOverlapCVar != nullptr, TEXT("Tried to disable console variable \"ai.debug.zonegraph.generation.RemoveOverlap\" for Tempo Lane Graph, but it was not found.")))
	{
		// Disable the removal of "overlapping" lanes.
		// We want a fully-connected intersection graph for now.
		// Later, we will prune the intersection graph more intelligently with tags, or other metadata.
		RemoveOverlapCVar->Set(false);
	}

	// Allow all Intersections to setup their data, first.
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		if (Actor->Implements<UTempoIntersectionInterface>())
		{
			FEditorScriptExecutionGuard ScriptGuard;
			ITempoIntersectionInterface::Execute_SetupTempoIntersectionData(Actor);
		}
	}
	
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		if (Actor->Implements<UTempoRoadInterface>())
		{
			DestroyZoneShapeComponents(*Actor);

			// First, try to generate ZoneShapeComponents for roads.
			if (!TryGenerateAndRegisterZoneShapeComponentsForRoad(*Actor))
			{
				UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Road ZoneShapeComponents for Actor: %s."), *Actor->GetName());
				return false;
			}
		}
		else if (Actor->Implements<UTempoRoadModuleInterface>())
		{
			DestroyZoneShapeComponents(*Actor);
			
			const AActor* RoadModuleParentActor = ITempoRoadModuleInterface::Execute_GetTempoRoadModuleParentActor(Actor);
			if (RoadModuleParentActor == nullptr)
			{
				UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Road Module ZoneShapeComponents for Actor: %s.  It must have a valid parent."), *Actor->GetName());
				return false;
			}

			// Build ZoneShapes for RoadModules differently, based on whether its parent is a Road or an Intersection.
			if (!RoadModuleParentActor->Implements<UTempoIntersectionInterface>())
			{
				// Then, try to generate ZoneShapeComponents for road modules (ex. sidewalks, walkable bridges, etc.).
				if (!TryGenerateAndRegisterZoneShapeComponentsForRoad(*Actor, true))
				{
					UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Road Module ZoneShapeComponents for Actor: %s."), *Actor->GetName());
					return false;
				}
			}
		}
		else if (Actor->Implements<UTempoIntersectionInterface>())
		{
			DestroyZoneShapeComponents(*Actor);

			if (!TryGenerateAndRegisterZoneShapeComponentsForIntersection(*Actor))
			{
				UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Intersection ZoneShapeComponents for Actor: %s."), *Actor->GetName());
				return false;
			}

			if (Actor->Implements<UTempoCrosswalkInterface>())
			{
				if (!TryGenerateAndRegisterZoneShapeComponentsForCrosswalksAtIntersections(*Actor))
				{
					UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Crosswalk ZoneShapeComponents for Actor: %s."), *Actor->GetName());
					return false;
				}

				if (!TryGenerateAndRegisterZoneShapeComponentsForCrosswalkIntersectionConnectorSegments(*Actor))
				{
					UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Crosswalk Intersection Connector Segment ZoneShapeComponents for Actor: %s."), *Actor->GetName());
					return false;
				}
			
				if (!TryGenerateAndRegisterZoneShapeComponentsForCrosswalkIntersections(*Actor))
				{
					UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Crosswalk Intersection ZoneShapeComponents for Actor: %s."), *Actor->GetName());
					return false;
				}
			}
		}
	}

	return true;
}

void UTempoRoadLaneGraphSubsystem::BuildZoneGraph() const
{
	UE::ZoneGraphDelegates::OnZoneGraphRequestRebuild.Broadcast();
}

//
// Road functions
//

bool UTempoRoadLaneGraphSubsystem::TryGenerateAndRegisterZoneShapeComponentsForRoad(AActor& RoadQueryActor, bool bQueryActorIsRoadModule) const
{
	TArray<UZoneShapeComponent*> ZoneShapeComponents;

	const bool bShouldGenerateZoneShapes = bQueryActorIsRoadModule
		? ITempoRoadModuleInterface::Execute_ShouldGenerateZoneShapesForTempoRoadModule(&RoadQueryActor)
		: ITempoRoadInterface::Execute_ShouldGenerateZoneShapesForTempoRoad(&RoadQueryActor);

	if (!bShouldGenerateZoneShapes)
	{
		// Allow the process to continue since we're meant to skip generating ZoneShapeComponents for this RoadQueryActor.
		UE_LOG(LogTempoAgentsEditor, Display, TEXT("Tempo Lane Graph - Skip generating ZoneShapeComponents for Actor: %s."), *RoadQueryActor.GetName());
		return true;
	}

	const FZoneLaneProfile LaneProfile = GetLaneProfile(RoadQueryActor, bQueryActorIsRoadModule);
	if (!LaneProfile.IsValid())
	{
		UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Road Zone Shape Components - Couldn't get valid LaneProfile for Actor: %s."), *RoadQueryActor.GetName());
		return false;
	}

	const int32 NumControlPoints = bQueryActorIsRoadModule
		? ITempoRoadModuleInterface::Execute_GetNumTempoRoadModuleControlPoints(&RoadQueryActor)
		: ITempoRoadInterface::Execute_GetNumTempoControlPoints(&RoadQueryActor);
	
	const bool bIsClosedLoop = bQueryActorIsRoadModule
		? ITempoRoadModuleInterface::Execute_IsTempoRoadModuleClosedLoop(&RoadQueryActor)
		: ITempoRoadInterface::Execute_IsTempoLaneClosedLoop(&RoadQueryActor);
	
	const float SampleDistanceStepSize = bQueryActorIsRoadModule
		? ITempoRoadModuleInterface::Execute_GetTempoRoadModuleSampleDistanceStepSize(&RoadQueryActor)
		: ITempoRoadInterface::Execute_GetTempoRoadSampleDistanceStepSize(&RoadQueryActor);

	// Generate ZoneShapeComponents and Setup their Points.
	if (bIsClosedLoop)
	{
		if (!ensureMsgf(NumControlPoints >= 3, TEXT("Expected NumControlPoints >= 3 in TryGenerateAndRegisterZoneShapeComponentsForRoad in closed-loop case.")))
		{
			return false;
		}
		
		float PrevControlPointDistanceAlongRoad = -1.0f;
		
		for (int32 CurrentControlPointIndex = 0; CurrentControlPointIndex < NumControlPoints; ++CurrentControlPointIndex)
		{
			const int32 NextControlPointIndex = CurrentControlPointIndex + 1;

			const float CurrentControlPointDistanceAlongRoad = bQueryActorIsRoadModule
				? ITempoRoadModuleInterface::Execute_GetDistanceAlongTempoRoadModuleAtControlPoint(&RoadQueryActor, CurrentControlPointIndex)
				: ITempoRoadInterface::Execute_GetDistanceAlongTempoRoadAtControlPoint(&RoadQueryActor, CurrentControlPointIndex);
		
			const float NextControlPointDistanceAlongRoad = bQueryActorIsRoadModule
				? ITempoRoadModuleInterface::Execute_GetDistanceAlongTempoRoadModuleAtControlPoint(&RoadQueryActor, NextControlPointIndex)
				: ITempoRoadInterface::Execute_GetDistanceAlongTempoRoadAtControlPoint(&RoadQueryActor, NextControlPointIndex);

			UZoneShapeComponent* ZoneShapeComponent = nullptr;

			if (!TryGenerateZoneShapeComponentBetweenDistancesAlongRoad(
				RoadQueryActor,
				CurrentControlPointDistanceAlongRoad,
				NextControlPointDistanceAlongRoad,
				SampleDistanceStepSize,
				LaneProfile,
				bQueryActorIsRoadModule,
				ZoneShapeComponent,
				&PrevControlPointDistanceAlongRoad))
			{
				ensureMsgf(false, TEXT("Failed to generate ZoneShapeComponent with ZoneShapePoints between distances along road (for closed-loop case) when building Lane Graph."));
				return false;
			}

			ZoneShapeComponents.Add(ZoneShapeComponent);
		}

		//
		// The remaining logic in this case is just to make sure the tangent at the closed-loop "seam"
		// is consistent for the *overall* first and last point.
		//

		UZoneShapeComponent* FirstZoneShapeComponent = ZoneShapeComponents.IsValidIndex(0) ? ZoneShapeComponents[0] : nullptr;
		UZoneShapeComponent* LastZoneShapeComponent = ZoneShapeComponents.IsValidIndex(NumControlPoints - 1) ? ZoneShapeComponents[NumControlPoints - 1] : nullptr;
		
		if (!ensureMsgf(FirstZoneShapeComponent != nullptr && LastZoneShapeComponent != nullptr, TEXT("Must get valid FirstZoneShapeComponent and LastZoneShapeComponent in TryGenerateAndRegisterZoneShapeComponentsForRoad for closed-loop case.")))
		{
			return false;
		}
		
		TArray<FZoneShapePoint>& FirstZoneShapeComponentPoints = FirstZoneShapeComponent->GetMutablePoints();
		TArray<FZoneShapePoint>& LastZoneShapeComponentPoints = LastZoneShapeComponent->GetMutablePoints();
		
		constexpr int32 FirstZoneShapePointIndex = 0;
		const int32 LastZoneShapePointIndex = LastZoneShapeComponent->GetNumPoints() - 1;
		
		FZoneShapePoint* FirstZoneShapePoint = FirstZoneShapeComponentPoints.IsValidIndex(FirstZoneShapePointIndex) ? &FirstZoneShapeComponentPoints[FirstZoneShapePointIndex] : nullptr;
		FZoneShapePoint* LastZoneShapePoint = LastZoneShapeComponentPoints.IsValidIndex(LastZoneShapePointIndex) ? &LastZoneShapeComponentPoints[LastZoneShapePointIndex] : nullptr;
		
		if (!ensureMsgf(FirstZoneShapePoint != nullptr && LastZoneShapePoint != nullptr, TEXT("Must get valid FirstZoneShapePoint and LastZoneShapePoint in TryGenerateAndRegisterZoneShapeComponentsForRoad for closed-loop case.")))
		{
			return false;
		}
		
		const FZoneShapePoint* PrevZoneShapePoint = LastZoneShapeComponentPoints.IsValidIndex(LastZoneShapePointIndex - 1) ? &LastZoneShapeComponentPoints[LastZoneShapePointIndex - 1] : nullptr;
		const FZoneShapePoint* NextZoneShapePoint = FirstZoneShapeComponentPoints.IsValidIndex(FirstZoneShapePointIndex + 1) ? &FirstZoneShapeComponentPoints[FirstZoneShapePointIndex + 1] : nullptr;
		
		if (!ensureMsgf(PrevZoneShapePoint != nullptr && NextZoneShapePoint != nullptr, TEXT("Must get valid PrevZoneShapePoint and NextZoneShapePoint in TryGenerateAndRegisterZoneShapeComponentsForRoad for closed-loop case.")))
		{
			return false;
		}
		
		const FVector ClosedLoopSeamTangent = (NextZoneShapePoint->Position - PrevZoneShapePoint->Position) * 0.5f / 3.0f;
		
		FirstZoneShapePoint->TangentLength = ClosedLoopSeamTangent.Size();
		
		LastZoneShapePoint->Rotation = FirstZoneShapePoint->Rotation;
		LastZoneShapePoint->TangentLength = FirstZoneShapePoint->TangentLength;
	}
	else
	{
		if (!ensureMsgf(NumControlPoints >= 2, TEXT("Expected NumControlPoints >= 2 in TryGenerateAndRegisterZoneShapeComponentsForRoad.")))
		{
			return false;
		}
		
		const int32 ControlPointStartIndex = bQueryActorIsRoadModule
			? 0
			: ITempoRoadInterface::Execute_GetTempoStartEntranceLocationControlPointIndex(&RoadQueryActor);
		
		const int32 ControlPointEndIndex = bQueryActorIsRoadModule
			? NumControlPoints - 1
			: ITempoRoadInterface::Execute_GetTempoEndEntranceLocationControlPointIndex(&RoadQueryActor);

		const float StartDistanceAlongRoad = bQueryActorIsRoadModule
			? ITempoRoadModuleInterface::Execute_GetDistanceAlongTempoRoadModuleAtControlPoint(&RoadQueryActor, ControlPointStartIndex)
			: ITempoRoadInterface::Execute_GetDistanceAlongTempoRoadAtControlPoint(&RoadQueryActor, ControlPointStartIndex);
		
		const float EndDistanceAlongRoad = bQueryActorIsRoadModule
			? ITempoRoadModuleInterface::Execute_GetDistanceAlongTempoRoadModuleAtControlPoint(&RoadQueryActor, ControlPointEndIndex)
			: ITempoRoadInterface::Execute_GetDistanceAlongTempoRoadAtControlPoint(&RoadQueryActor, ControlPointEndIndex);

		UZoneShapeComponent* ZoneShapeComponent = nullptr;

		if (!TryGenerateZoneShapeComponentBetweenDistancesAlongRoad(
			RoadQueryActor,
			StartDistanceAlongRoad,
			EndDistanceAlongRoad,
			SampleDistanceStepSize,
			LaneProfile,
			bQueryActorIsRoadModule,
			ZoneShapeComponent))
		{
			ensureMsgf(false, TEXT("Failed to generate ZoneShapeComponent with ZoneShapePoints between distances along road when building Lane Graph."));
			return false;
		}

		ZoneShapeComponents.Add(ZoneShapeComponent);
	}

	// Register ZoneShapeComponents.
	for (UZoneShapeComponent* ZoneShapeComponent : ZoneShapeComponents)
	{
		if (!TryRegisterZoneShapeComponentWithActor(RoadQueryActor, *ZoneShapeComponent))
		{
			return false;
		}
	}

	return true;
}

bool UTempoRoadLaneGraphSubsystem::TryGenerateZoneShapeComponentBetweenDistancesAlongRoad(AActor& RoadQueryActor, float StartDistanceAlongRoad, float EndDistanceAlongRoad, float TargetSampleDistanceStepSize, const FZoneLaneProfile& LaneProfile, bool bQueryActorIsRoadModule, UZoneShapeComponent*& OutZoneShapeComponent, float* InOutPrevSampleDistance, AActor* OverrideZoneShapeComponentOwnerActor) const
{
	AActor& ZoneShapeComponentOwnerActor = OverrideZoneShapeComponentOwnerActor != nullptr ? *OverrideZoneShapeComponentOwnerActor : RoadQueryActor;
	
	UZoneShapeComponent* ZoneShapeComponent = NewObject<UZoneShapeComponent>(&ZoneShapeComponentOwnerActor, UZoneShapeComponent::StaticClass());
	if (!ensureMsgf(ZoneShapeComponent != nullptr, TEXT("Failed to create ZoneShapeComponent in TryGenerateZoneShapeComponentBetweenDistancesAlongRoad when building Lane Graph.")))
	{
		return false;
	}
	
	const FZoneLaneProfileRef LaneProfileRef(LaneProfile);
	
	// Remove default points.
	ZoneShapeComponent->GetMutablePoints().Empty();
	ZoneShapeComponent->SetCommonLaneProfile(LaneProfileRef);

	// Limit the smallest step size to something reasonable.
	TargetSampleDistanceStepSize = FMath::Max(TargetSampleDistanceStepSize, 10.0f);

	// Set SampleDistanceStepSize to the closet value <= TargetSampleDistanceStepSize
	// such that we generate a uniformly-spaced set of samples.
	const float TotalZoneShapeLength = EndDistanceAlongRoad - StartDistanceAlongRoad;
	const int32 NumUniformSegments = FMath::CeilToInt(TotalZoneShapeLength / TargetSampleDistanceStepSize);
	const float SampleDistanceStepSize = TotalZoneShapeLength / NumUniformSegments;

	float PrevSampleDistance = InOutPrevSampleDistance != nullptr && *InOutPrevSampleDistance >= 0.0f ? *InOutPrevSampleDistance : StartDistanceAlongRoad;
	
	for (int32 CurrentSampleIndex = 0; CurrentSampleIndex <= NumUniformSegments; ++CurrentSampleIndex)
	{
		const float CurrentSampleDistance = FMath::Min(SampleDistanceStepSize * CurrentSampleIndex + StartDistanceAlongRoad, EndDistanceAlongRoad);
		const float NextSampleDistance = FMath::Min(CurrentSampleDistance + SampleDistanceStepSize, EndDistanceAlongRoad);
		
		const FVector PrevSampleLocation = bQueryActorIsRoadModule
			? ITempoRoadModuleInterface::Execute_GetLocationAtDistanceAlongTempoRoadModule(&RoadQueryActor, PrevSampleDistance, ETempoCoordinateSpace::Local)
			: ITempoRoadInterface::Execute_GetLocationAtDistanceAlongTempoRoad(&RoadQueryActor, PrevSampleDistance, ETempoCoordinateSpace::Local);
		
		const FVector NextSampleLocation = bQueryActorIsRoadModule
			? ITempoRoadModuleInterface::Execute_GetLocationAtDistanceAlongTempoRoadModule(&RoadQueryActor, NextSampleDistance, ETempoCoordinateSpace::Local)
			: ITempoRoadInterface::Execute_GetLocationAtDistanceAlongTempoRoad(&RoadQueryActor, NextSampleDistance, ETempoCoordinateSpace::Local);
		
		const FVector CurrentSampleTangent = (NextSampleLocation - PrevSampleLocation) * 0.5f / 3.0f;
		const float CurrentSampleTangentLength = CurrentSampleTangent.Size();
		
		FZoneShapePoint ZoneShapePoint = CreateZoneShapePointAtDistanceAlongRoad(RoadQueryActor, CurrentSampleDistance, CurrentSampleTangentLength, bQueryActorIsRoadModule);
		ZoneShapeComponent->GetMutablePoints().Add(ZoneShapePoint);

		// Don't bump PrevSampleDistance the last time through the loop,
		// so we can output the final "previous" sample distance below.
		if (CurrentSampleIndex < NumUniformSegments)
		{
			PrevSampleDistance = CurrentSampleDistance;
		}
	}

	OutZoneShapeComponent = ZoneShapeComponent;

	if (InOutPrevSampleDistance != nullptr)
	{
		*InOutPrevSampleDistance = PrevSampleDistance;
	}

	return true;
}

FZoneShapePoint UTempoRoadLaneGraphSubsystem::CreateZoneShapePointAtDistanceAlongRoad(const AActor& RoadQueryActor, float DistanceAlongRoad, float TangentLength, bool bQueryActorIsRoadModule) const
{
	FZoneShapePoint ZoneShapePoint;
	
	const FVector ZoneShapePointLocation = bQueryActorIsRoadModule
		? ITempoRoadModuleInterface::Execute_GetLocationAtDistanceAlongTempoRoadModule(&RoadQueryActor, DistanceAlongRoad, ETempoCoordinateSpace::Local)
		: ITempoRoadInterface::Execute_GetLocationAtDistanceAlongTempoRoad(&RoadQueryActor, DistanceAlongRoad, ETempoCoordinateSpace::Local);

	const FRotator ZoneShapePointRotation = bQueryActorIsRoadModule
		? ITempoRoadModuleInterface::Execute_GetRotationAtDistanceAlongTempoRoadModule(&RoadQueryActor, DistanceAlongRoad, ETempoCoordinateSpace::Local)
		: ITempoRoadInterface::Execute_GetRotationAtDistanceAlongTempoRoad(&RoadQueryActor, DistanceAlongRoad, ETempoCoordinateSpace::Local);

	ZoneShapePoint.Position = ZoneShapePointLocation;
	ZoneShapePoint.Rotation = ZoneShapePointRotation;
	ZoneShapePoint.Type = FZoneShapePointType::Bezier;
	
	ZoneShapePoint.TangentLength = TangentLength;

	return ZoneShapePoint;
}

FZoneLaneProfile UTempoRoadLaneGraphSubsystem::CreateDynamicLaneProfile(const AActor& RoadQueryActor, bool bQueryActorIsRoadModule) const
{
	FZoneLaneProfile LaneProfile;

	const int32 NumLanes = bQueryActorIsRoadModule
		? ITempoRoadModuleInterface::Execute_GetNumTempoRoadModuleLanes(&RoadQueryActor)
		: ITempoRoadInterface::Execute_GetNumTempoLanes(&RoadQueryActor);

	for (int32 LaneIndex = 0; LaneIndex < NumLanes; ++LaneIndex)
	{
		// ZoneGraph's LaneProfiles specify lanes from right to left.
		const int32 LaneProfileLaneIndex = NumLanes - 1 - LaneIndex;
		
		const float LaneWidth = bQueryActorIsRoadModule
			? ITempoRoadModuleInterface::Execute_GetTempoRoadModuleLaneWidth(&RoadQueryActor, LaneProfileLaneIndex)
			: ITempoRoadInterface::Execute_GetTempoLaneWidth(&RoadQueryActor, LaneProfileLaneIndex);
		
		const EZoneLaneDirection LaneDirection = bQueryActorIsRoadModule
			? ITempoRoadModuleInterface::Execute_GetTempoRoadModuleLaneDirection(&RoadQueryActor, LaneProfileLaneIndex)
			: ITempoRoadInterface::Execute_GetTempoLaneDirection(&RoadQueryActor, LaneProfileLaneIndex);
		
		const TArray<FName> LaneTags = bQueryActorIsRoadModule
			? ITempoRoadModuleInterface::Execute_GetTempoRoadModuleLaneTags(&RoadQueryActor, LaneProfileLaneIndex)
			: ITempoRoadInterface::Execute_GetTempoLaneTags(&RoadQueryActor, LaneProfileLaneIndex);

		FZoneLaneDesc LaneDesc = CreateZoneLaneDesc(LaneWidth, LaneDirection, LaneTags);
		LaneProfile.Lanes.Add(LaneDesc);
	}

	LaneProfile.Name = GenerateDynamicLaneProfileName(LaneProfile);

	return LaneProfile;
}

FZoneLaneDesc UTempoRoadLaneGraphSubsystem::CreateZoneLaneDesc(const float LaneWidth, const EZoneLaneDirection LaneDirection, const TArray<FName>& LaneTagNames) const
{
	FZoneLaneDesc LaneDesc;
	
	LaneDesc.Width = LaneWidth;
	LaneDesc.Direction = LaneDirection;

	FZoneGraphTagMask LaneTagMask;
	for (FName LaneTagName : LaneTagNames)
	{
		FZoneGraphTag LaneTag = GetTagByName(LaneTagName);
		if (LaneTag != FZoneGraphTag::None)
		{
			LaneTagMask.Add(LaneTag);
		}
	}

	LaneDesc.Tags = LaneTagMask;

	return LaneDesc;
}

FName UTempoRoadLaneGraphSubsystem::GenerateDynamicLaneProfileName(const FZoneLaneProfile& LaneProfile) const
{
	FString LaneProfileBodyString;
	
	for (const FZoneLaneDesc& ZoneLaneDesc : LaneProfile.Lanes)
	{
		const float LaneWidth = ZoneLaneDesc.Width;
		const FString DirectionLetter = ZoneLaneDesc.Direction == EZoneLaneDirection::Forward ? TEXT("f") :
										ZoneLaneDesc.Direction == EZoneLaneDirection::Backward ? TEXT("b") :
										TEXT("");
		const int32 LaneTagMask = ZoneLaneDesc.Tags.GetValue();
		
		LaneProfileBodyString += FString::Printf(TEXT("_%.4f%st%d"), LaneWidth, *DirectionLetter, LaneTagMask);
	}

	const FString LaneProfileNameString = !LaneProfileBodyString.IsEmpty() ? FString::Printf(TEXT("DynamicLaneProfile_%s"), *LaneProfileBodyString) : FString();
	const FName LaneProfileName = FName(LaneProfileNameString);
	
	return LaneProfileName;
}

bool UTempoRoadLaneGraphSubsystem::TryWriteDynamicLaneProfile(const FZoneLaneProfile& LaneProfile) const
{
	UTempoZoneGraphSettings* MutableTempoZoneGraphSettings = GetMutableTempoZoneGraphSettings();
	if (MutableTempoZoneGraphSettings == nullptr)
	{
		return false;
	}

	// Only write the dynamic lane profile, if it doesn't already exist in the settings.
	FZoneLaneProfile LaneProfileFromSettings;
	if (!TryGetDynamicLaneProfileFromSettings(LaneProfile, LaneProfileFromSettings))
	{
		TArray<FZoneLaneProfile>& MutableLaneProfiles = MutableTempoZoneGraphSettings->GetMutableLaneProfiles();
		MutableLaneProfiles.Add(LaneProfile);

		UZoneGraphSettings* MutableBaseZoneGraphSettings = GetMutableDefault<UZoneGraphSettings>();
		MutableBaseZoneGraphSettings->SaveConfig(CPF_Config, *MutableBaseZoneGraphSettings->GetDefaultConfigFilename());
	}

	return true;
}

bool UTempoRoadLaneGraphSubsystem::TryGetDynamicLaneProfileFromSettings(const FZoneLaneProfile& InLaneProfile, FZoneLaneProfile& OutLaneProfileFromSettings) const
{
	const UZoneGraphSettings* ZoneGraphSettings = GetDefault<UZoneGraphSettings>();
	if (ZoneGraphSettings == nullptr)
	{
		return false;
	}

	const TArray<FZoneLaneProfile>& LaneProfiles = ZoneGraphSettings->GetLaneProfiles();
	const FZoneLaneProfile* LaneProfileFromSettings = LaneProfiles.FindByPredicate([&InLaneProfile](const FZoneLaneProfile& LaneProfileInArray)
	{
		return LaneProfileInArray.Name == InLaneProfile.Name;
	});

	if (LaneProfileFromSettings == nullptr)
	{
		return false;
	}

	OutLaneProfileFromSettings = *LaneProfileFromSettings;

	return true;
}

FZoneLaneProfile UTempoRoadLaneGraphSubsystem::GetLaneProfile(const AActor& RoadQueryActor, bool bQueryActorIsRoadModule) const
{
	const FName LaneProfileOverrideName = bQueryActorIsRoadModule
		? ITempoRoadModuleInterface::Execute_GetTempoRoadModuleLaneProfileOverrideName(&RoadQueryActor)
		: ITempoRoadInterface::Execute_GetTempoLaneProfileOverrideName(&RoadQueryActor);

	if (LaneProfileOverrideName != NAME_None)
	{
		const FZoneLaneProfile OverrideLaneProfile = GetLaneProfileByName(LaneProfileOverrideName);
		return OverrideLaneProfile;
	}

	const FZoneLaneProfile DynamicLaneProfile = CreateDynamicLaneProfile(RoadQueryActor, bQueryActorIsRoadModule);
	
	FZoneLaneProfile DynamicLaneProfileFromSettings;
	if (TryGetDynamicLaneProfileFromSettings(DynamicLaneProfile, DynamicLaneProfileFromSettings))
	{
		// The Lane Profile stored in the settings will have a unique ID (ie. Guid).
		// So, we always want to reference that one, if it exists.
		return DynamicLaneProfileFromSettings;
	}
	else
	{
		if (!TryWriteDynamicLaneProfile(DynamicLaneProfile))
		{
			UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to write Dynamic Lane Profile for Actor: %s.  Returning default constructed profile."), *RoadQueryActor.GetName());
			return FZoneLaneProfile();
		}
	}
	
	return DynamicLaneProfile;
}

FZoneLaneProfile UTempoRoadLaneGraphSubsystem::GetLaneProfileByName(FName LaneProfileName) const
{
	const UZoneGraphSettings* ZoneGraphSettings = GetDefault<UZoneGraphSettings>();

	if (ZoneGraphSettings == nullptr)
	{
		return FZoneLaneProfile();
	}

	for (const FZoneLaneProfile& LaneProfile : ZoneGraphSettings->GetLaneProfiles())
	{
		if (LaneProfile.Name == LaneProfileName)
		{
			return LaneProfile;
		}
	}

	return FZoneLaneProfile();
}

//
// Intersection functions
//

bool UTempoRoadLaneGraphSubsystem::TryGenerateAndRegisterZoneShapeComponentsForIntersection(AActor& IntersectionQueryActor) const
{
	UZoneShapeComponent* ZoneShapeComponent = NewObject<UZoneShapeComponent>(&IntersectionQueryActor, UZoneShapeComponent::StaticClass());
	if (!ensureMsgf(ZoneShapeComponent != nullptr, TEXT("Failed to create ZoneShapeComponent when building Intersection for Actor: %s."), *IntersectionQueryActor.GetName()))
	{
		return false;
	}

	// Remove default points.
	ZoneShapeComponent->GetMutablePoints().Empty();
	
	ZoneShapeComponent->SetShapeType(FZoneShapeType::Polygon);
	ZoneShapeComponent->SetPolygonRoutingType(EZoneShapePolygonRoutingType::Bezier);

	// Apply intersection tags.
	TArray<FName> IntersectionTagNames = ITempoIntersectionInterface::Execute_GetTempoIntersectionTags(&IntersectionQueryActor);
	for (FName IntersectionTagName : IntersectionTagNames)
	{
		const FZoneGraphTag IntersectionTag = GetTagByName(IntersectionTagName);
		ZoneShapeComponent->GetMutableTags().Add(IntersectionTag);
	}
	
	const int32 NumConnections = ITempoIntersectionInterface::Execute_GetNumTempoConnections(&IntersectionQueryActor);

	// Create and Setup Points.
	for (int32 ConnectionIndex = 0; ConnectionIndex < NumConnections; ++ConnectionIndex)
	{
		FZoneShapePoint ZoneShapePoint;
		if (!TryCreateZoneShapePointForIntersectionEntranceLocation(IntersectionQueryActor, ConnectionIndex, *ZoneShapeComponent, ZoneShapePoint))
		{
			return false;
		}

		ZoneShapeComponent->GetMutablePoints().Add(ZoneShapePoint);
	}

	// Register ZoneShapeComponent.
	if (!TryRegisterZoneShapeComponentWithActor(IntersectionQueryActor, *ZoneShapeComponent))
	{
		return false;
	}

	return true;
}

bool UTempoRoadLaneGraphSubsystem::TryCreateZoneShapePointForIntersectionEntranceLocation(const AActor& IntersectionQueryActor, int32 ConnectionIndex, UZoneShapeComponent& ZoneShapeComponent, FZoneShapePoint& OutZoneShapePoint) const
{
	const FVector IntersectionEntranceLocationInWorldFrame = ITempoIntersectionInterface::Execute_GetTempoIntersectionEntranceLocation(&IntersectionQueryActor, ConnectionIndex, ETempoCoordinateSpace::World);
	
	DrawDebugSphere(GetWorld(), IntersectionEntranceLocationInWorldFrame, 50.0f, 32, FColor::Green, false, 5.0f);

	AActor* RoadQueryActor = GetConnectedRoadActor(IntersectionQueryActor, ConnectionIndex);

	if (RoadQueryActor == nullptr)
	{
		UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Intersection Zone Shape Point - Failed to get Connected Road Actor for Actor: %s at ConnectionIndex: %d."), *IntersectionQueryActor.GetName(), ConnectionIndex);
		return false;
	}
	
	const FZoneLaneProfile LaneProfile = GetLaneProfile(*RoadQueryActor, false);
	if (!LaneProfile.IsValid())
	{
		UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Intersection Zone Shape Point - Couldn't get valid LaneProfile for Connected Road Actor for Actor: %s at ConnectionIndex: %d."), *IntersectionQueryActor.GetName(), ConnectionIndex);
		return false;
	}
	
	const FZoneLaneProfileRef LaneProfileRef(LaneProfile);

	const uint8 PerPointLaneProfileIndex = ZoneShapeComponent.AddUniquePerPointLaneProfile(LaneProfileRef);

	FZoneShapePoint ZoneShapePoint;
	
	ZoneShapePoint.Position = IntersectionQueryActor.ActorToWorld().InverseTransformPosition(IntersectionEntranceLocationInWorldFrame);
	ZoneShapePoint.Type = FZoneShapePointType::LaneProfile;
	ZoneShapePoint.LaneProfile = PerPointLaneProfileIndex;
	ZoneShapePoint.TangentLength = LaneProfile.GetLanesTotalWidth() * 0.5f;

	const FVector IntersectionEntranceTangentInWorldFrame = ITempoIntersectionInterface::Execute_GetTempoIntersectionEntranceTangent(&IntersectionQueryActor, ConnectionIndex, ETempoCoordinateSpace::World);
	const FVector IntersectionEntranceUpVectorInWorldFrame = ITempoIntersectionInterface::Execute_GetTempoIntersectionEntranceUpVector(&IntersectionQueryActor, ConnectionIndex, ETempoCoordinateSpace::World);

	ZoneShapePoint.SetRotationFromForwardAndUp(ZoneShapeComponent.GetComponentToWorld().InverseTransformPosition(IntersectionEntranceTangentInWorldFrame),
		ZoneShapeComponent.GetComponentToWorld().InverseTransformPosition(IntersectionEntranceUpVectorInWorldFrame));

	DrawDebugDirectionalArrow(GetWorld(), IntersectionQueryActor.ActorToWorld().TransformPosition(ZoneShapePoint.GetInControlPoint()), IntersectionQueryActor.ActorToWorld().TransformPosition(ZoneShapePoint.Position), 10000.0f, FColor::Blue, false, 10.0f, 0, 10.0f);
	DrawDebugDirectionalArrow(GetWorld(), IntersectionQueryActor.ActorToWorld().TransformPosition(ZoneShapePoint.Position), IntersectionQueryActor.ActorToWorld().TransformPosition(ZoneShapePoint.GetOutControlPoint()), 10000.0f, FColor::Red, false, 10.0f, 0, 10.0f);

	const int32 IntersectionEntranceControlPointIndex = ITempoIntersectionInterface::Execute_GetTempoIntersectionEntranceControlPointIndex(&IntersectionQueryActor, ConnectionIndex);
	const FVector RoadTangentAtIntersectionEntranceInWorldFrame = ITempoRoadInterface::Execute_GetTempoControlPointTangent(RoadQueryActor, IntersectionEntranceControlPointIndex, ETempoCoordinateSpace::World);
	
	const bool bIsRoadLeavingIntersection = RoadTangentAtIntersectionEntranceInWorldFrame.Dot(IntersectionEntranceTangentInWorldFrame) < 0.0f;
	if (bIsRoadLeavingIntersection)
	{
		ZoneShapePoint.bReverseLaneProfile = true;
	}

	OutZoneShapePoint = ZoneShapePoint;

	return true;
}

AActor* UTempoRoadLaneGraphSubsystem::GetConnectedRoadActor(const AActor& IntersectionQueryActor, int32 ConnectionIndex) const
{
	AActor* RoadActor = ITempoIntersectionInterface::Execute_GetConnectedTempoRoadActor(&IntersectionQueryActor, ConnectionIndex);
			
	if (RoadActor == nullptr)
	{
		return nullptr;
	}

	if (!RoadActor->Implements<UTempoRoadInterface>())
	{
		return nullptr;
	}

	return RoadActor;
}

//
// Crosswalk functions
//

bool UTempoRoadLaneGraphSubsystem::TryGenerateAndRegisterZoneShapeComponentsForCrosswalksAtIntersections(AActor& CrosswalkQueryActor) const
{
	const int32 NumConnections = ITempoIntersectionInterface::Execute_GetNumTempoConnections(&CrosswalkQueryActor);
	
	for (int32 ConnectionIndex = 0; ConnectionIndex < NumConnections; ++ConnectionIndex)
	{
		const bool bShouldGenerateZoneShapesForCrosswalk = ITempoCrosswalkInterface::Execute_ShouldGenerateZoneShapesForTempoCrosswalk(&CrosswalkQueryActor, ConnectionIndex);

		if (!bShouldGenerateZoneShapesForCrosswalk)
		{
			UE_LOG(LogTempoAgentsEditor, Display, TEXT("Tempo Lane Graph - Skip generating Crosswalk ZoneShapeComponents for Actor: %s at ConnectionIndex: %d."), *CrosswalkQueryActor.GetName(), ConnectionIndex);
			continue;
		}
		
		const FZoneLaneProfile LaneProfile = GetLaneProfileForCrosswalk(CrosswalkQueryActor, ConnectionIndex);
		if (!LaneProfile.IsValid())
		{
			UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Crosswalk ZoneShapeComponent - Couldn't get valid LaneProfile for Actor: %s at ConnectionIndex: %d."), *CrosswalkQueryActor.GetName(), ConnectionIndex);
			return false;
		}
	
		const FZoneLaneProfileRef LaneProfileRef(LaneProfile);
		
		UZoneShapeComponent* ZoneShapeComponent = NewObject<UZoneShapeComponent>(&CrosswalkQueryActor, UZoneShapeComponent::StaticClass());
		if (!ensureMsgf(ZoneShapeComponent != nullptr, TEXT("Failed to create Crosswalk ZoneShapeComponent when building Lane Graph for Actor: %s at ConnectionIndex: %d."), *CrosswalkQueryActor.GetName(), ConnectionIndex))
		{
			return false;
		}

		// Remove default points.
		ZoneShapeComponent->GetMutablePoints().Empty();
		ZoneShapeComponent->SetCommonLaneProfile(LaneProfileRef);

		// Apply crosswalk tags.
		TArray<FName> CrosswalkTagNames = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkTags(&CrosswalkQueryActor, ConnectionIndex);
		for (const FName& CrosswalkTagName : CrosswalkTagNames)
		{
			const FZoneGraphTag CrosswalkTag = GetTagByName(CrosswalkTagName);
			ZoneShapeComponent->GetMutableTags().Add(CrosswalkTag);
		}

		const int32 CrosswalkControlPointStartIndex = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkStartEntranceLocationControlPointIndex(&CrosswalkQueryActor, ConnectionIndex);
		const int32 CrosswalkControlPointEndIndex = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkEndEntranceLocationControlPointIndex(&CrosswalkQueryActor, ConnectionIndex);
		
		for (int32 CrosswalkControlPointIndex = CrosswalkControlPointStartIndex; CrosswalkControlPointIndex <= CrosswalkControlPointEndIndex; ++CrosswalkControlPointIndex)
		{
			FZoneShapePoint ZoneShapePoint;
			if (!TryCreateZoneShapePointForCrosswalkControlPoint(CrosswalkQueryActor, ConnectionIndex, CrosswalkControlPointIndex, ZoneShapePoint))
			{
				return false;
			}
			
			ZoneShapeComponent->GetMutablePoints().Add(ZoneShapePoint);
		}

		// Register ZoneShapeComponent.
		if (!TryRegisterZoneShapeComponentWithActor(CrosswalkQueryActor, *ZoneShapeComponent))
		{
			return false;
		}
	}

	return true;
}

bool UTempoRoadLaneGraphSubsystem::TryGenerateAndRegisterZoneShapeComponentsForCrosswalkIntersectionConnectorSegments(AActor& CrosswalkQueryActor) const
{
	const auto& GetNumControlPointsBetweenDistancesAlongRoadModule = [](const AActor& RoadModuleQueryActor, float StartDistance, float EndDistance)
	{
		TArray<int32> RoadModuleControlPointIndexes;

		if (StartDistance > EndDistance)
		{
			return 0;
		}

		const float RoadModuleLength = ITempoRoadModuleInterface::Execute_GetTempoRoadModuleLength(&RoadModuleQueryActor);

		StartDistance = FMath::Clamp(StartDistance, 0.0f, RoadModuleLength);
		EndDistance = FMath::Clamp(EndDistance, 0.0f, RoadModuleLength);

		const int32 NumRoadModuleControlPoints = ITempoRoadModuleInterface::Execute_GetNumTempoRoadModuleControlPoints(&RoadModuleQueryActor);

		for (int32 RoadModuleControlPointIndex = 0; RoadModuleControlPointIndex < NumRoadModuleControlPoints; ++RoadModuleControlPointIndex)
		{
			const float DistanceAlongRoadModule = ITempoRoadModuleInterface::Execute_GetDistanceAlongTempoRoadModuleAtControlPoint(&RoadModuleQueryActor, RoadModuleControlPointIndex);

			if (DistanceAlongRoadModule >= StartDistance && DistanceAlongRoadModule <= EndDistance)
			{
				RoadModuleControlPointIndexes.Add(RoadModuleControlPointIndex);
			}
		}

		return RoadModuleControlPointIndexes.Num();
	};
	
	const int32 NumCrosswalkRoadModules = ITempoCrosswalkInterface::Execute_GetNumConnectedTempoCrosswalkRoadModules(&CrosswalkQueryActor);

	for (int32 CrosswalkRoadModuleIndex = 0; CrosswalkRoadModuleIndex < NumCrosswalkRoadModules; ++CrosswalkRoadModuleIndex)
	{
		const FCrosswalkIntersectionConnectorInfo& CrosswalkIntersectionConnectorInfo = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionConnectorInfo(&CrosswalkQueryActor, CrosswalkRoadModuleIndex);

		if (!ensureMsgf(CrosswalkIntersectionConnectorInfo.CrosswalkRoadModule != nullptr, TEXT("Must get valid CrosswalkRoadModule in UTempoRoadLaneGraphSubsystem::TryGenerateAndRegisterZoneShapeComponentsForCrosswalkIntersectionConnectorSegments.")))
		{
			return false;
		}
		
		const FZoneLaneProfile LaneProfile = GetCrosswalkIntersectionConnectorLaneProfile(CrosswalkQueryActor, CrosswalkRoadModuleIndex);
		
		if (!LaneProfile.IsValid())
		{
			UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Crosswalk Intersection Connector ZoneShapeComponent - Couldn't get valid LaneProfile for Actor: %s."), *CrosswalkIntersectionConnectorInfo.CrosswalkRoadModule->GetName());
			return false;
		}

		for (const FCrosswalkIntersectionConnectorSegmentInfo& CrosswalkIntersectionConnectorSegmentInfo : CrosswalkIntersectionConnectorInfo.CrosswalkIntersectionConnectorSegmentInfos)
		{
			const int32 NumRoadModuleControlPointIndexesForSegment = GetNumControlPointsBetweenDistancesAlongRoadModule(
				*CrosswalkIntersectionConnectorInfo.CrosswalkRoadModule,
				CrosswalkIntersectionConnectorSegmentInfo.CrosswalkIntersectionConnectorStartDistance,
				CrosswalkIntersectionConnectorSegmentInfo.CrosswalkIntersectionConnectorEndDistance);

			const int32 NumSegmentControlPoints = FMath::Max(NumRoadModuleControlPointIndexesForSegment, 2);
			const float SegmentLength = CrosswalkIntersectionConnectorSegmentInfo.CrosswalkIntersectionConnectorEndDistance - CrosswalkIntersectionConnectorSegmentInfo.CrosswalkIntersectionConnectorStartDistance;

			const float SegmentStep = SegmentLength / NumSegmentControlPoints;

			UZoneShapeComponent* ZoneShapeComponent = nullptr;
			
			if (!TryGenerateZoneShapeComponentBetweenDistancesAlongRoad(
				*CrosswalkIntersectionConnectorInfo.CrosswalkRoadModule,
				CrosswalkIntersectionConnectorSegmentInfo.CrosswalkIntersectionConnectorStartDistance,
				CrosswalkIntersectionConnectorSegmentInfo.CrosswalkIntersectionConnectorEndDistance,
				SegmentStep,
				LaneProfile,
				true,
				ZoneShapeComponent,
				nullptr,
				&CrosswalkQueryActor))
			{
				ensureMsgf(false, TEXT("Failed to create Crosswalk Intersection Connector Segment ZoneShapeComponent when building Lane Graph for Actor: %s at CrosswalkRoadModuleIndex: %d."), *CrosswalkQueryActor.GetName(), CrosswalkRoadModuleIndex);
				return false;
			}

			// Register ZoneShapeComponent.
			if (!TryRegisterZoneShapeComponentWithActor(CrosswalkQueryActor, *ZoneShapeComponent))
			{
				return false;
			}
		}
	}

	return true;
}

bool UTempoRoadLaneGraphSubsystem::TryGenerateAndRegisterZoneShapeComponentsForCrosswalkIntersections(AActor& CrosswalkQueryActor) const
{
	const int32 NumCrosswalkIntersections = ITempoCrosswalkInterface::Execute_GetNumTempoCrosswalkIntersections(&CrosswalkQueryActor);

	for (int32 CrosswalkIntersectionIndex = 0; CrosswalkIntersectionIndex < NumCrosswalkIntersections; ++CrosswalkIntersectionIndex)
	{
		UZoneShapeComponent* ZoneShapeComponent = NewObject<UZoneShapeComponent>(&CrosswalkQueryActor, UZoneShapeComponent::StaticClass());

		if (!ensureMsgf(ZoneShapeComponent != nullptr, TEXT("Failed to create Crosswalk Intersection ZoneShapeComponent when building Lane Graph for Actor: %s at CrosswalkIntersectionIndex: %d."), *CrosswalkQueryActor.GetName(), CrosswalkIntersectionIndex))
		{
			return false;
		}

		// Remove default points.
		ZoneShapeComponent->GetMutablePoints().Empty();
	
		ZoneShapeComponent->SetShapeType(FZoneShapeType::Polygon);
		ZoneShapeComponent->SetPolygonRoutingType(EZoneShapePolygonRoutingType::Bezier);

		// Apply crosswalk intersection tags.
		TArray<FName> IntersectionTagNames = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionTags(&CrosswalkQueryActor, CrosswalkIntersectionIndex);
		for (FName IntersectionTagName : IntersectionTagNames)
		{
			const FZoneGraphTag IntersectionTag = GetTagByName(IntersectionTagName);
			ZoneShapeComponent->GetMutableTags().Add(IntersectionTag);
		}
	
		const int32 NumCrosswalkIntersectionConnections = ITempoCrosswalkInterface::Execute_GetNumTempoCrosswalkIntersectionConnections(&CrosswalkQueryActor, CrosswalkIntersectionIndex);

		// Create and Setup Points.
		for (int32 CrosswalkIntersectionConnectionIndex = 0; CrosswalkIntersectionConnectionIndex < NumCrosswalkIntersectionConnections; ++CrosswalkIntersectionConnectionIndex)
		{
			FZoneShapePoint ZoneShapePoint;
			if (!TryCreateZoneShapePointForCrosswalkIntersectionEntranceLocation(CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex, *ZoneShapeComponent, ZoneShapePoint))
			{
				return false;
			}

			ZoneShapeComponent->GetMutablePoints().Add(ZoneShapePoint);
		}

		// Register ZoneShapeComponent.
		if (!TryRegisterZoneShapeComponentWithActor(CrosswalkQueryActor, *ZoneShapeComponent))
		{
			return false;
		}
	}

	return true;
}

bool UTempoRoadLaneGraphSubsystem::TryCreateZoneShapePointForCrosswalkIntersectionEntranceLocation(const AActor& CrosswalkQueryActor, int32 CrosswalkIntersectionIndex, int32 CrosswalkIntersectionConnectionIndex, UZoneShapeComponent& ZoneShapeComponent, FZoneShapePoint& OutZoneShapePoint) const
{
	const FVector CrosswalkIntersectionEntranceLocationInWorldFrame = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionEntranceLocation(&CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex, ETempoCoordinateSpace::World);
	
	DrawDebugSphere(GetWorld(), CrosswalkIntersectionEntranceLocationInWorldFrame, 25.0f, 16, FColor::Green, false, 5.0f);
	
	const int32 CrosswalkRoadModuleIndex = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkRoadModuleIndexFromCrosswalkIntersectionIndex(&CrosswalkQueryActor, CrosswalkIntersectionIndex);

	AActor* CrosswalkRoadModuleQueryActor = ITempoCrosswalkInterface::Execute_GetConnectedTempoCrosswalkRoadModuleActor(&CrosswalkQueryActor, CrosswalkRoadModuleIndex);

	if (CrosswalkRoadModuleQueryActor == nullptr)
	{
		UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Crosswalk Intersection Zone Shape Point - Failed to get Connected Road Actor for Actor: %s at CrosswalkRoadModuleIndex: %d."), *CrosswalkQueryActor.GetName(), CrosswalkRoadModuleIndex);
		return false;
	}
	
	const FZoneLaneProfile LaneProfile = GetCrosswalkIntersectionEntranceLaneProfile(CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex);
	if (!LaneProfile.IsValid())
	{
		UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to create Crosswalk Intersection Zone Shape Point - Couldn't get valid LaneProfile for Actor: %s at CrosswalkIntersectionIndex: %d CrosswalkIntersectionConnectionIndex: %d."), *CrosswalkQueryActor.GetName(), CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex);
		return false;
	}
	
	const FZoneLaneProfileRef LaneProfileRef(LaneProfile);

	const uint8 PerPointLaneProfileIndex = ZoneShapeComponent.AddUniquePerPointLaneProfile(LaneProfileRef);

	FZoneShapePoint ZoneShapePoint;
	
	ZoneShapePoint.Position = CrosswalkQueryActor.ActorToWorld().InverseTransformPosition(CrosswalkIntersectionEntranceLocationInWorldFrame);
	ZoneShapePoint.Type = FZoneShapePointType::LaneProfile;
	ZoneShapePoint.LaneProfile = PerPointLaneProfileIndex;
	ZoneShapePoint.TangentLength = LaneProfile.GetLanesTotalWidth() * 0.5f;

	const FVector CrosswalkIntersectionEntranceTangentInWorldFrame = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionEntranceTangent(&CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex, ETempoCoordinateSpace::World);
	const FVector CrosswalkIntersectionEntranceUpVectorInWorldFrame = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionEntranceUpVector(&CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex, ETempoCoordinateSpace::World);

	ZoneShapePoint.SetRotationFromForwardAndUp(ZoneShapeComponent.GetComponentToWorld().InverseTransformPosition(CrosswalkIntersectionEntranceTangentInWorldFrame),
		ZoneShapeComponent.GetComponentToWorld().InverseTransformPosition(CrosswalkIntersectionEntranceUpVectorInWorldFrame));

	DrawDebugDirectionalArrow(GetWorld(), CrosswalkQueryActor.ActorToWorld().TransformPosition(ZoneShapePoint.GetInControlPoint()), CrosswalkQueryActor.ActorToWorld().TransformPosition(ZoneShapePoint.Position), 10000.0f, FColor::Blue, false, 10.0f, 0, 10.0f);
	DrawDebugDirectionalArrow(GetWorld(), CrosswalkQueryActor.ActorToWorld().TransformPosition(ZoneShapePoint.Position), CrosswalkQueryActor.ActorToWorld().TransformPosition(ZoneShapePoint.GetOutControlPoint()), 10000.0f, FColor::Red, false, 10.0f, 0, 10.0f);

	OutZoneShapePoint = ZoneShapePoint;

	return true;
}

bool UTempoRoadLaneGraphSubsystem::TryCreateZoneShapePointForCrosswalkControlPoint(const AActor& CrosswalkQueryActor, int32 ConnectionIndex, int32 CrosswalkControlPointIndex, FZoneShapePoint& OutZoneShapePoint) const
{
	FZoneShapePoint ZoneShapePoint;
	
	const FVector CrosswalkControlPointLocation = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkControlPointLocation(&CrosswalkQueryActor, ConnectionIndex, CrosswalkControlPointIndex, ETempoCoordinateSpace::World);
	const FVector CrosswalkControlPointTangent = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkControlPointTangent(&CrosswalkQueryActor, ConnectionIndex, CrosswalkControlPointIndex, ETempoCoordinateSpace::World);

	const FVector CrosswalkControlPointForwardVector = CrosswalkControlPointTangent.GetSafeNormal();
	const FVector CrosswalkControlPointUpVector = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkControlPointUpVector(&CrosswalkQueryActor, ConnectionIndex, CrosswalkControlPointIndex, ETempoCoordinateSpace::World);

	ZoneShapePoint.Position = CrosswalkQueryActor.GetTransform().InverseTransformPosition(CrosswalkControlPointLocation);
	ZoneShapePoint.Type = FZoneShapePointType::Bezier;
	ZoneShapePoint.TangentLength = CrosswalkControlPointTangent.Size();
	
	ZoneShapePoint.SetRotationFromForwardAndUp(CrosswalkQueryActor.GetTransform().InverseTransformVector(CrosswalkControlPointForwardVector),
		CrosswalkQueryActor.GetTransform().InverseTransformVector(CrosswalkControlPointUpVector));

	OutZoneShapePoint = ZoneShapePoint;
	
	return true;
}

FZoneLaneProfile UTempoRoadLaneGraphSubsystem::CreateDynamicLaneProfileForCrosswalk(const AActor& CrosswalkQueryActor, int32 ConnectionIndex) const
{
	FZoneLaneProfile LaneProfile;
	
	const int32 NumLanes = ITempoCrosswalkInterface::Execute_GetNumTempoCrosswalkLanes(&CrosswalkQueryActor, ConnectionIndex);

	for (int32 LaneIndex = 0; LaneIndex < NumLanes; ++LaneIndex)
	{
		// ZoneGraph's LaneProfiles specify lanes from right to left.
		const int32 LaneProfileLaneIndex = NumLanes - 1 - LaneIndex;
		
		const float LaneWidth = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkLaneWidth(&CrosswalkQueryActor, ConnectionIndex, LaneProfileLaneIndex);
		const EZoneLaneDirection LaneDirection = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkLaneDirection(&CrosswalkQueryActor, ConnectionIndex, LaneProfileLaneIndex);
		const TArray<FName> LaneTags = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkLaneTags(&CrosswalkQueryActor, ConnectionIndex, LaneProfileLaneIndex);

		FZoneLaneDesc LaneDesc = CreateZoneLaneDesc(LaneWidth, LaneDirection, LaneTags);
		LaneProfile.Lanes.Add(LaneDesc);
	}

	LaneProfile.Name = GenerateDynamicLaneProfileName(LaneProfile);

	return LaneProfile;
}

FZoneLaneProfile UTempoRoadLaneGraphSubsystem::GetLaneProfileForCrosswalk(const AActor& CrosswalkQueryActor, int32 ConnectionIndex) const
{
	const FName LaneProfileOverrideName = ITempoCrosswalkInterface::Execute_GetLaneProfileOverrideNameForTempoCrosswalk(&CrosswalkQueryActor, ConnectionIndex);

	if (LaneProfileOverrideName != NAME_None)
	{
		const FZoneLaneProfile OverrideLaneProfile = GetLaneProfileByName(LaneProfileOverrideName);
		return OverrideLaneProfile;
	}

	const FZoneLaneProfile DynamicLaneProfile = CreateDynamicLaneProfileForCrosswalk(CrosswalkQueryActor, ConnectionIndex);
	
	FZoneLaneProfile DynamicLaneProfileFromSettings;
	if (TryGetDynamicLaneProfileFromSettings(DynamicLaneProfile, DynamicLaneProfileFromSettings))
	{
		// The Lane Profile stored in the settings will have a unique ID (ie. Guid).
		// So, we always want to reference that one, if it exists.
		return DynamicLaneProfileFromSettings;
	}
	else
	{
		if (!TryWriteDynamicLaneProfile(DynamicLaneProfile))
		{
			UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to write Dynamic Lane Profile for Crosswalk for Actor: %s.  Returning default constructed profile."), *CrosswalkQueryActor.GetName());
			return FZoneLaneProfile();
		}
	}
	
	return DynamicLaneProfile;
}

FZoneLaneProfile UTempoRoadLaneGraphSubsystem::CreateCrosswalkIntersectionConnectorDynamicLaneProfile(const AActor& CrosswalkQueryActor, int32 CrosswalkRoadModuleIndex) const
{
	FZoneLaneProfile LaneProfile;
	
	const int32 NumLanes = ITempoCrosswalkInterface::Execute_GetNumTempoCrosswalkIntersectionConnectorLanes(&CrosswalkQueryActor, CrosswalkRoadModuleIndex);

	for (int32 LaneIndex = 0; LaneIndex < NumLanes; ++LaneIndex)
	{
		// ZoneGraph's LaneProfiles specify lanes from right to left.
		const int32 LaneProfileLaneIndex = NumLanes - 1 - LaneIndex;
		
		const float LaneWidth = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionConnectorLaneWidth(&CrosswalkQueryActor, CrosswalkRoadModuleIndex, LaneProfileLaneIndex);
		const EZoneLaneDirection LaneDirection = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionConnectorLaneDirection(&CrosswalkQueryActor, CrosswalkRoadModuleIndex, LaneProfileLaneIndex);
		const TArray<FName> LaneTags = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionConnectorLaneTags(&CrosswalkQueryActor, CrosswalkRoadModuleIndex, LaneProfileLaneIndex);

		FZoneLaneDesc LaneDesc = CreateZoneLaneDesc(LaneWidth, LaneDirection, LaneTags);
		LaneProfile.Lanes.Add(LaneDesc);
	}

	LaneProfile.Name = GenerateDynamicLaneProfileName(LaneProfile);

	return LaneProfile;
}

FZoneLaneProfile UTempoRoadLaneGraphSubsystem::GetCrosswalkIntersectionConnectorLaneProfile(const AActor& CrosswalkQueryActor, int32 CrosswalkRoadModuleIndex) const
{
	const FName LaneProfileOverrideName = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionConnectorLaneProfileOverrideName(&CrosswalkQueryActor, CrosswalkRoadModuleIndex);

	if (LaneProfileOverrideName != NAME_None)
	{
		const FZoneLaneProfile OverrideLaneProfile = GetLaneProfileByName(LaneProfileOverrideName);
		return OverrideLaneProfile;
	}
	
	const FZoneLaneProfile DynamicLaneProfile = CreateCrosswalkIntersectionConnectorDynamicLaneProfile(CrosswalkQueryActor, CrosswalkRoadModuleIndex);
	
	FZoneLaneProfile DynamicLaneProfileFromSettings;
	if (TryGetDynamicLaneProfileFromSettings(DynamicLaneProfile, DynamicLaneProfileFromSettings))
	{
		// The Lane Profile stored in the settings will have a unique ID (ie. Guid).
		// So, we always want to reference that one, if it exists.
		return DynamicLaneProfileFromSettings;
	}
	else
	{
		if (!TryWriteDynamicLaneProfile(DynamicLaneProfile))
		{
			UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to write Dynamic Lane Profile for Crosswalk for Actor: %s.  Returning default constructed profile."), *CrosswalkQueryActor.GetName());
			return FZoneLaneProfile();
		}
	}
	
	return DynamicLaneProfile;
}

FZoneLaneProfile UTempoRoadLaneGraphSubsystem::CreateCrosswalkIntersectionEntranceDynamicLaneProfile(const AActor& CrosswalkQueryActor, int32 CrosswalkIntersectionIndex, int32 CrosswalkIntersectionConnectionIndex) const
{
	FZoneLaneProfile LaneProfile;
	
	const int32 NumLanes = ITempoCrosswalkInterface::Execute_GetNumTempoCrosswalkIntersectionEntranceLanes(&CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex);

	for (int32 LaneIndex = 0; LaneIndex < NumLanes; ++LaneIndex)
	{
		// ZoneGraph's LaneProfiles specify lanes from right to left.
		const int32 LaneProfileLaneIndex = NumLanes - 1 - LaneIndex;
		
		const float LaneWidth = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionEntranceLaneWidth(&CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex, LaneProfileLaneIndex);
		const EZoneLaneDirection LaneDirection = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionEntranceLaneDirection(&CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex, LaneProfileLaneIndex);
		const TArray<FName> LaneTags = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionEntranceLaneTags(&CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex, LaneProfileLaneIndex);

		FZoneLaneDesc LaneDesc = CreateZoneLaneDesc(LaneWidth, LaneDirection, LaneTags);
		LaneProfile.Lanes.Add(LaneDesc);
	}

	LaneProfile.Name = GenerateDynamicLaneProfileName(LaneProfile);

	return LaneProfile;
}

FZoneLaneProfile UTempoRoadLaneGraphSubsystem::GetCrosswalkIntersectionEntranceLaneProfile(const AActor& CrosswalkQueryActor, int32 CrosswalkIntersectionIndex, int32 CrosswalkIntersectionConnectionIndex) const
{
	const FName LaneProfileOverrideName = ITempoCrosswalkInterface::Execute_GetTempoCrosswalkIntersectionEntranceLaneProfileOverrideName(&CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex);

	if (LaneProfileOverrideName != NAME_None)
	{
		const FZoneLaneProfile OverrideLaneProfile = GetLaneProfileByName(LaneProfileOverrideName);
		return OverrideLaneProfile;
	}
	
	const FZoneLaneProfile DynamicLaneProfile = CreateCrosswalkIntersectionEntranceDynamicLaneProfile(CrosswalkQueryActor, CrosswalkIntersectionIndex, CrosswalkIntersectionConnectionIndex);
	
	FZoneLaneProfile DynamicLaneProfileFromSettings;
	if (TryGetDynamicLaneProfileFromSettings(DynamicLaneProfile, DynamicLaneProfileFromSettings))
	{
		// The Lane Profile stored in the settings will have a unique ID (ie. Guid).
		// So, we always want to reference that one, if it exists.
		return DynamicLaneProfileFromSettings;
	}
	else
	{
		if (!TryWriteDynamicLaneProfile(DynamicLaneProfile))
		{
			UE_LOG(LogTempoAgentsEditor, Error, TEXT("Tempo Lane Graph - Failed to write Dynamic Lane Profile for Crosswalk for Actor: %s.  Returning default constructed profile."), *CrosswalkQueryActor.GetName());
			return FZoneLaneProfile();
		}
	}
	
	return DynamicLaneProfile;
}

//
// Shared functions
//

bool UTempoRoadLaneGraphSubsystem::TryRegisterZoneShapeComponentWithActor(AActor& Actor, UZoneShapeComponent& ZoneShapeComponent) const
{
	if (USceneComponent* RootComponent = Actor.GetRootComponent())
	{
		ZoneShapeComponent.SetupAttachment(RootComponent);
	}
	else
	{
		if (!Actor.SetRootComponent(&ZoneShapeComponent))
		{
			return false;
		}
	}
	
	ZoneShapeComponent.RegisterComponent();
	Actor.AddInstanceComponent(&ZoneShapeComponent);

	return true;
}

void UTempoRoadLaneGraphSubsystem::DestroyZoneShapeComponents(AActor& Actor) const
{
	TArray<UZoneShapeComponent*> Components;
	Actor.GetComponents<UZoneShapeComponent>(Components);
	
	for (UZoneShapeComponent* Component : Components)
	{
		Component->DestroyComponent();
	}
}

FZoneGraphTag UTempoRoadLaneGraphSubsystem::GetTagByName(const FName TagName) const
{
	FZoneGraphTag ZoneGraphTag = FZoneGraphTag::None;
	
	const UZoneGraphSubsystem* ZoneGraphSubsystem = UWorld::GetSubsystem<UZoneGraphSubsystem>(GetWorld());

	if (ensureMsgf(ZoneGraphSubsystem != nullptr, TEXT("Can't access UZoneGraphSubsystem.")))
	{
		ZoneGraphTag = ZoneGraphSubsystem->GetTagByName(TagName);
	}

	return ZoneGraphTag;
}

UTempoZoneGraphSettings* UTempoRoadLaneGraphSubsystem::GetMutableTempoZoneGraphSettings() const
{
	UZoneGraphSettings* MutableBaseZoneGraphSettings = GetMutableDefault<UZoneGraphSettings>();

	// This allows us to add mutable accessors to the ZoneGraphSettings without modifying the ZoneGraph source.
	UTempoZoneGraphSettings* MutableTempoZoneGraphSettings = static_cast<UTempoZoneGraphSettings*>(MutableBaseZoneGraphSettings);

	return MutableTempoZoneGraphSettings;
}

UWorld* UTempoRoadLaneGraphSubsystem::GetWorld() const
{
	UWorld* GameWorld = const_cast<UTempoRoadLaneGraphSubsystem*>(this)->GetGameWorld();
	UWorld* EditorWorld = const_cast<UTempoRoadLaneGraphSubsystem*>(this)->GetEditorWorld();

	return GameWorld != nullptr ? GameWorld : EditorWorld;
}
