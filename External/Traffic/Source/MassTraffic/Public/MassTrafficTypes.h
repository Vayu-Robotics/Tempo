// Copyright Epic Games, Inc. All Rights Reserved.


#pragma once

#include "MassTraffic.h"
#include "ZoneGraphTypes.h"

#include "HierarchicalHashGrid2D.h"
#include "MassEntityView.h"
#include "MassTrafficSigns.h"

#include "Containers/Set.h"

#include "MassTrafficTypes.generated.h"

#define MASSTRAFFIC_NUM_INLINE_VEHICLE_NEXT_LANES 2 // ..determined to be ~1.4 on average for this game
#define MASSTRAFFIC_NUM_INLINE_VEHICLE_PREV_LANES 2 // Should be similar to merging lanes.
#define MASSTRAFFIC_NUM_INLINE_VEHICLE_MERGING_LANES 2 // ..determined to be ~1.3 on average for this game
#define MASSTRAFFIC_NUM_INLINE_VEHICLE_SPLITTING_LANES 2 // ..determined to be ~1.7 on average for this game
#define MASSTRAFFIC_NUM_INLINE_VEHICLE_CONFLICT_LANES 16 // There could potentially be many more conflict lanes.  We'll need to keep an eye on this.
#define MASSTRAFFIC_NUM_INLINE_VEHICLE_CROSSWALK_LANES 12 // Each lane should only run through (at most) this many crosswalk lanes.
#define MASSTRAFFIC_NUM_INLINE_VEHICLE_CROSSWALKS 6 // Each lane should only run through (at most) this many crosswalks.


namespace UE::MassTraffic
{

typedef THierarchicalHashGrid2D<1, 1, int32/*lane index*/> FMassTrafficBasicHGrid;


// Positive unit float stored as an unsigned int type.
#define DEBUG_FRACTION_TEMPLATE 0

template<bool DoAutoBoundDefault, typename StorageType>
struct MASSTRAFFIC_API TFraction
{
public:
	
	TFraction()
	{
	}

	explicit TFraction(const float Value, const bool bDoAutoBound = DoAutoBoundDefault)
	{
		Set(Value, bDoAutoBound);
	}

	explicit TFraction(const TFraction& RHS)
	{
		InternalDataAsIntType = RHS.InternalDataAsIntType;
	}

	
	FORCEINLINE /*explicit*/ operator float() const
	{
		return Get();
	}

	FORCEINLINE TFraction& operator=(const TFraction& RHS)
	{
		InternalDataAsIntType = RHS.InternalDataAsIntType;
		return *this;
	}
	
	FORCEINLINE float operator=(const float Value)
	{
		Set(Value);
		return Get();
	}
	
	FORCEINLINE void Set(float Value, const bool bDoAutoBound = DoAutoBoundDefault)
	{
		ensureAlwaysMsgf(bDoAutoBound || Value >= 0.0f && Value <= 1.0f, TEXT("Value %f outside range [0.0 .. 1.0]. Clamping."), Value);
		
		Value = FMath::Clamp(Value, 0.0f, 1.0f);

#if DEBUG_FRACTION_TEMPLATE
		InternalDataAsFloatForDebug = Fraction;
#else
		if (Value <= 0.0f) InternalDataAsIntType = static_cast<StorageType>(0);
		else if (Value >= 1.0f) InternalDataAsIntType = MaxInternalDataAsIntType;
		else InternalDataAsIntType = static_cast<StorageType>(MaxInternalDataAsFloat * Value) + 1; // .."+1"? See comment for InternalValue member.

#endif
	}

	FORCEINLINE float Get() const
	{
#if DEBUG_FRACTION_TEMPLATE
		return InternalDataAsFloatForDebug;
#else
		// "-1?" See comment for InternalValue member.
		return !InternalDataAsIntType ? 0.0f : static_cast<float>(InternalDataAsIntType) / MaxInternalDataAsFloat;
#endif
	}
	
private:

	/**
	 * We want this class to be able to store something non-zero when asked to store tiny positive floating point values,
	 * ones that would normally round off to an integer value of 0 for the type T we are using. To allow this, we store
	 * non-zero values from 1 to MAX<T>-1, not MAX<T>. Internal values of 0 really only correspond to 0.0. Internal values
	 * of MAX<T> correspond to 1.0. Internal values of 1 are the smallest non-zero value this class can store, and
	 * correspond to 1.0/(MAX<T>-1). See the Set() and Get() methods.
	 * Example -
	 * If I were to call Set(0.0f), this class will set it's internal value to 0.
	 * If I were to call Set(1.0f), this class will set it's internal value to MAX<T>-1.
	 * If I were to call Set(0.000000001f), this class will set it's internal value to 1.
	 * If I then call Get() after that last Set() call, I'll get back 1.0/(MAX<T>-1).
	 */
#if DEBUG_FRACTION_TEMPLATE
	float InternalDataAsFloatForDebug = 0.0f;
#else
	StorageType InternalDataAsIntType = static_cast<StorageType>(0);
	static inline const StorageType MaxInternalDataAsIntType = TNumericLimits<StorageType>::Max() - static_cast<StorageType>(1);
	static inline const float MaxInternalDataAsFloat = static_cast<float>(MaxInternalDataAsIntType);
#endif
};


/**
 * Positive unit float stored as an unsigned int type - supporting an additional scale and offset.
 * NOTE - Scale and offset should really be a floats, but templates won't take floats as parameters. Instead, I'm
 * providing ints for numerators and denominators. May switch this to <ratio>s in the future.
 */
template<int32 LowNumerator, int32 LowDenominator, int32 HighNumerator, int32 HighDenominator, bool DoAutoBoundDefault, typename StorageType> // ..see comment above
struct MASSTRAFFIC_API TRangeFraction : TFraction<DoAutoBoundDefault, StorageType>
{
public:
	
	TRangeFraction()
	{
	}

	explicit TRangeFraction(const float Value, const bool bDoAutoBound = DoAutoBoundDefault)
	{
		Set(Value, bDoAutoBound);
	}

	explicit TRangeFraction(const TRangeFraction& RHS) :
		TFraction<DoAutoBoundDefault, StorageType>(RHS)
	{
	}

	
	FORCEINLINE static float Low() 
	{
		ensureAlwaysMsgf(LowDenominator != 0, TEXT("Low denominator is zero."));

		return static_cast<float>(LowNumerator) / static_cast<float>(LowDenominator);		
	}

	FORCEINLINE static float High() 
	{
		ensureAlwaysMsgf(HighDenominator != 0, TEXT("High denominator is zero."));

		return static_cast<float>(HighNumerator) / static_cast<float>(HighDenominator);		
	}

	
	FORCEINLINE /*explicit*/ operator float() const
	{
		return Get();
	}

	FORCEINLINE float operator=(const float Value) 
	{
		Set(Value);
		return Get();
	}
	
	FORCEINLINE void Set(const float Value, const bool bDoAutoBound = DoAutoBoundDefault)
	{
		const float Low = TFraction<DoAutoBoundDefault, StorageType>::Low();
		const float High = TFraction<DoAutoBoundDefault, StorageType>::High();
		ensureAlwaysMsgf(Low != High, TEXT("Low:%f == High:%f."), Low, High);
		
		TFraction<DoAutoBoundDefault, StorageType>::Set((Value - Low) / (High - Low), bDoAutoBound);
	}

	FORCEINLINE float Get() const
	{
		const float Low = TFraction<DoAutoBoundDefault, StorageType>::Low();
		const float High = TFraction<DoAutoBoundDefault, StorageType>::High();
		return TFraction<DoAutoBoundDefault, StorageType>::Get() * (High - Low) + Low;
	}
};

/** A faster, smaller-footprint list for a small number of entities. But will be very slow for larger sizes. */
template<int32 MAX>
class TSmallEntityList : public TStaticArray<FMassEntityHandle, MAX, sizeof(FMassEntityHandle)>
{
public:
	
	TSmallEntityList() :
		TStaticArray<FMassEntityHandle, MAX, sizeof(FMassEntityHandle)>()
	{
	}

	explicit TSmallEntityList(const FMassEntityHandle Entity) :
		TStaticArray<FMassEntityHandle, MAX, sizeof(FMassEntityHandle)>(Entity)
	{
	}

	explicit TSmallEntityList(TStaticArray<FMassEntityHandle, MAX, sizeof(FMassEntityHandle)>&& Other) :
		TStaticArray<FMassEntityHandle, MAX, sizeof(FMassEntityHandle)>(Other)
	{
	}

	explicit TSmallEntityList(const TStaticArray<FMassEntityHandle, MAX, sizeof(FMassEntityHandle)>& Other) :
		TStaticArray<FMassEntityHandle, MAX, sizeof(FMassEntityHandle)>(Other)
	{
	}

	
	FORCEINLINE static int32 Max()
	{
		return MAX;		
	}
	
	
	FORCEINLINE int32 NumValid() const
	{
		int32 CountValid = 0;
		for (int32 I = 0; I < MAX; I++)
		{
			if ((*this)[I].IsSet())
			{
				++CountValid;
			}
		}
		
		return CountValid;		
	}

	FORCEINLINE bool IsEmpty() const
	{
		// Faster than checking NumValid().
		for (int32 I = 0; I < MAX; I++)
		{
			if ((*this)[I].IsSet())
			{
				return false;
			}
		}

		return true;
	}

	FORCEINLINE bool IsFull() const
	{
		// Faster than checking NumValid().
		for (int32 I = 0; I < MAX; I++)
		{
			if (!(*this)[I].IsSet())
			{
				return false;
			}
		}

		return true;
	}

	FORCEINLINE bool Contains(const FMassEntityHandle Entity) const
	{
		for (int32 I = 0; I < MAX; I++)
		{
			if ((*this)[I] == Entity)
			{
				return true;
			}
		}

		return false;
	}

	FORCEINLINE int32 Find(const FMassEntityHandle Entity) const
	{
		int32 Count = 0;
		for (int32 I = 0; I < MAX; I++)
		{
			if ((*this)[I] == Entity)
			{
				++Count;
			}
		}

		return Count;
	}
	
	FORCEINLINE bool AddUnique(const FMassEntityHandle Entity)
	{
		// No need to check if full first.
		
		if (Contains(Entity))
		{
			return false;
		}
		
		for (int32 I = 0; I < MAX; I++)
		{
			if (!(*this)[I].IsSet())
			{
				(*this)[I] = Entity;
				return true;
			}
		}
		
		return false;
	}
	
	FORCEINLINE bool Add(const FMassEntityHandle Entity)
	{
		// No need to check if full first.
		
		for (int32 I = 0; I < MAX; I++)
		{
			if (!(*this)[I].IsSet())
			{
				(*this)[I] = Entity;
				return true;
			}
		}
		
		return false;
	}

	FORCEINLINE bool RemoveFirst(const FMassEntityHandle Entity)
	{
		for (int32 I = 0; I < MAX; I++)
		{
			if ((*this)[I].IsSet() && (*this)[I] == Entity)
			{
				(*this)[I] = FMassEntityHandle();
				return true;
			}
		}

		return false;
	}

	FORCEINLINE int32 RemoveAll(const FMassEntityHandle Entity)
	{
		int32 NumRemoved = 0;
		for (int32 I = 0; I < MAX; I++)
		{
			if ((*this)[I].IsSet() && (*this)[I] == Entity)
			{
				(*this)[I] = FMassEntityHandle();
				++NumRemoved;
			}
		}

		return NumRemoved;
	}

	FORCEINLINE void Empty()
	{
		for (int32 I = 0; I < MAX; I++)
		{
			(*this)[I] = FMassEntityHandle();
		}
	}
};

/** Lane turn type. */
enum class LaneTurnType : uint8
{
	Straight = 0x0,
	LeftTurn = 0x1,
	RightTurn = 0x2,
};

}


/**
 * Struct to store a float value with an ID.
 * Can work as a TMap key.
 */
USTRUCT()
struct MASSTRAFFIC_API FMassTrafficFloatAndID
{
	GENERATED_BODY()

	FMassTrafficFloatAndID(float InValue = 0.0f, int32 InID = INDEX_NONE) :
		Value(InValue),
		ID(InID)
	{
	}

	UPROPERTY()
	float Value = 0.0f;

	UPROPERTY()
	int32 ID = INDEX_NONE;
};

static bool operator==(const FMassTrafficFloatAndID& LHS, const FMassTrafficFloatAndID& RHS)
{
	return LHS.Value == RHS.Value && LHS.ID == RHS.ID;
}

static bool operator<(const FMassTrafficFloatAndID& LHS, const FMassTrafficFloatAndID& RHS)
{
	// Only sort on value, ignore ID.
	return LHS.Value < RHS.Value;
}

static uint32 GetTypeHash(const FMassTrafficFloatAndID& FloatAndID)
{
	uint32 Hash = 0x0; 
	Hash = HashCombine(Hash, GetTypeHash(FloatAndID.Value));
	Hash = HashCombine(Hash, GetTypeHash(FloatAndID.ID));
	return Hash;
}


struct MASSTRAFFIC_API FMassTrafficControllerType
{
	FMassTrafficControllerType(bool bIsTrafficLightControlledIn, EMassTrafficControllerSignType TrafficControllerSignTypeIn)
		: bIsTrafficLightControlled(bIsTrafficLightControlledIn), TrafficControllerSignType(TrafficControllerSignTypeIn)
	{
		ensureMsgf(!(bIsTrafficLightControlled && TrafficControllerSignType != EMassTrafficControllerSignType::None),
			TEXT("TrafficControllerSignType cannot be both traffic light and sign controlled."));
	}

	bool GetIsTrafficLightControlled() const { return bIsTrafficLightControlled; }
	EMassTrafficControllerSignType GetTrafficControllerSignType() const { return TrafficControllerSignType; }

private:
	bool bIsTrafficLightControlled = false;
	EMassTrafficControllerSignType TrafficControllerSignType = EMassTrafficControllerSignType::None;
};

/**
 * Constant lane data that each vehicle takes a copy of when entering the lane 
 */
struct MASSTRAFFIC_API FZoneGraphTrafficLaneConstData
{
	FZoneGraphTrafficLaneConstData() :
		bIsIntersectionLane(false),
		bIsTrunkLane(false),
		bIsLaneChangingLane(false)
	{
	}

	/**
	 * Lane is inside an intersection (an intersection interior lane)
	 * @see UMassTrafficSettings::IntersectionTag
	 */
	bool bIsIntersectionLane : 1;

	/**
	 * Lane is inside an intersection (an intersection interior lane)
	 * @see UMassTrafficSettings::TrunkLaneFilter
	 */
	bool bIsTrunkLane : 1;

	/**
	 * Lane changing is allowed on this lane
	 * @see UMassTrafficSettings::LaneChangingLaneFilter
	 */
	bool bIsLaneChangingLane : 1;

	/** Lane speed limit in cm/s */
	FFloat16 SpeedLimit = 0.0f;

	/**
	 * The minimum speed limit in cm/s across all this lanes connected 'next' lanes. Used to blend from SpeedLimit
	 * at the start of this lane to AverageNextLanesSpeedLimit at the end
	 */
	FFloat16 AverageNextLanesSpeedLimit = 0.0f;

	void AddTrafficController(float DistanceAlongLane, const FMassTrafficControllerType& TrafficControllerType)
	{
		TrafficControllerTypes.Add(TPair<float, FMassTrafficControllerType>(DistanceAlongLane, TrafficControllerType));
		TrafficControllerTypes.Sort([](const auto& Left, const auto& Right)
		{
			return Left.Key < Right.Key;
		});
	}

	TOptional<TPair<float, FMassTrafficControllerType>> TryGetTrafficControllerType(float DistanceAlongLane) const
	{
		for (const auto& Elem : TrafficControllerTypes)
		{
			const float ControllerDistance = Elem.Key;
			if (ControllerDistance > DistanceAlongLane)
			{
				return Elem;
			}
		}

		return TOptional<TPair<float, FMassTrafficControllerType>>();
	}

	TOptional<FMassTrafficControllerType> TryGetTrafficControllerTypeAtStart() const
	{
		if (!TrafficControllerTypes.IsEmpty() && TrafficControllerTypes[0].Key == 0)
		{
			return TrafficControllerTypes[0].Value;
		}

		return TOptional<FMassTrafficControllerType>();
	}

	TOptional<float> TryGetLaneLengthAtNextTrafficControl(float DistanceAlongLane) const
	{
		for (const auto& Elem : TrafficControllerTypes)
		{
			const float ControllerDistance = Elem.Key;
			if (ControllerDistance > DistanceAlongLane)
			{
				return ControllerDistance;
			}
		}

		return TOptional<float>();
	}

private:
	/**
	 * Traffic controller types at distances along this lane. Always sorted by distance.
	 */
	TArray<TPair<float, FMassTrafficControllerType>> TrafficControllerTypes;
};


typedef TFunction< bool (const FMassEntityView& VehicleEntityView, struct FMassTrafficNextVehicleFragment& NextVehicleFragment, struct FMassZoneGraphLaneLocationFragment& LaneLocationFragment) > FTrafficVehicleExecuteFunction;

USTRUCT()
struct MASSTRAFFIC_API FZoneGraphTrafficLaneData
{
	GENERATED_BODY()

	FZoneGraphTrafficLaneData();

	bool bIsOpen : 1;
	bool bIsAboutToClose : 1; // ..lane will close soon (within Coordinator's StandardTrafficPrepareToStopSeconds)
	bool bTurnsLeft : 1;
	bool bTurnsRight : 1;
	bool bIsRightMostLane : 1;
	bool bHasTransverseLaneAdjacency : 1;
	bool bIsDownstreamFromIntersection : 1;
	bool bIsStoppedVehicleInPreviousLaneOverlappingThisLane : 1; // (See all CROSSWALKOVERLAP.)
	
	// Note:  Currently only sign-controlled intersections will set this flag.
	TMap<float, bool, TSetAllocator<TSparseArrayAllocator<>, TInlineAllocator<MASSTRAFFIC_NUM_INLINE_VEHICLE_CROSSWALKS>>> HasPedestriansWaitingToCross;

	// Note:  Currently only sign-controlled intersections will set this flag.
	TMap<float, bool, TSetAllocator<TSparseArrayAllocator<>, TInlineAllocator<MASSTRAFFIC_NUM_INLINE_VEHICLE_CROSSWALKS>>> HasPedestriansInDownstreamCrosswalkLanes;

	// Note:  Currently only sign-controlled intersections will set this flag.
	TMap<float, bool, TSetAllocator<TSparseArrayAllocator<>, TInlineAllocator<MASSTRAFFIC_NUM_INLINE_VEHICLE_CROSSWALKS>>> AreAllEntitiesOnCrosswalkYielding;

	UE::MassTraffic::TFraction<true, uint8> FractionUntilClosed;

	/**
	 * Cached length of the zone graph lane
	 * @see UE::ZoneGraph::Query::GetLaneLength
	 */
	float Length = 0.0f;

	FZoneGraphLaneHandle LaneHandle;			// Zone Graph lane index, @see FZoneGraphStorage::Lanes
	FZoneGraphTrafficLaneConstData ConstData;	// Inline const data for vehicles to inline when they enter this lane
	float SpaceAvailable = 0.0f;				// ..values get too big for half float
	UE::MassTraffic::TFraction<false, uint8> MaxDensity;

	FMassEntityHandle TailVehicle;
	FMassEntityHandle GhostTailVehicle_FromLaneChangingVehicle;
	FMassEntityHandle GhostTailVehicle_FromSplittingLaneVehicle;
	FMassEntityHandle GhostTailVehicle_FromMergingLaneVehicle;
	
	uint8 NumVehiclesOnLane = 0;
	uint8 NumVehiclesApproachingLane = 0; 
	uint8 NumReservedVehiclesOnLane = 0; // See all CANTSTOPLANEEXIT.
	
	FMassEntityHandle IntersectionEntityHandle;
	
	FZoneGraphTrafficLaneData* LeftLane = nullptr; // ..non-merging non-splitting same-direction lane on left 
	FZoneGraphTrafficLaneData* RightLane = nullptr; // ..non-merging non-splitting same-direction lane on right
	TArray<FZoneGraphTrafficLaneData*, TInlineAllocator<MASSTRAFFIC_NUM_INLINE_VEHICLE_NEXT_LANES>> NextLanes;
	TArray<FZoneGraphTrafficLaneData*, TInlineAllocator<MASSTRAFFIC_NUM_INLINE_VEHICLE_PREV_LANES>> PrevLanes;
	TArray<FZoneGraphTrafficLaneData*, TInlineAllocator<MASSTRAFFIC_NUM_INLINE_VEHICLE_MERGING_LANES>> MergingLanes;
	TArray<FZoneGraphTrafficLaneData*, TInlineAllocator<MASSTRAFFIC_NUM_INLINE_VEHICLE_SPLITTING_LANES>> SplittingLanes;
	TArray<FZoneGraphTrafficLaneData*, TInlineAllocator<MASSTRAFFIC_NUM_INLINE_VEHICLE_CONFLICT_LANES>> ConflictLanes;

	// Lanes which are not drivable will not have an associated FZoneGraphTrafficLaneData.
	// So, we just store FZoneGraphLaneHandles for crosswalk lanes with "right of way" over this lane,
	// which we call "downstream crosswalk lanes".
	TMap<FZoneGraphLaneHandle, float, TSetAllocator<TSparseArrayAllocator<>, TInlineAllocator<MASSTRAFFIC_NUM_INLINE_VEHICLE_CROSSWALK_LANES>>> DownstreamCrosswalkLanes;

	/**
	 * NOTE - If these take up too much memory, we can instead make a single 1-bit flag to cover both of these, that simply
	 * tells if the lane is generally involved in a lane change - something like 'bIsLaneInvolvedInLaneChange', and remove
	 * these two counters. But this will also result in less lane changes and the loss of some finer behaviors.
	 * (See all LANECHANGEONOFF.)
	 */
	uint8 NumVehiclesLaneChangingOntoLane = 0;
	uint8 NumVehiclesLaneChangingOffOfLane = 0;

	/** Center location (average between start and end lane location) and radius for distance testing */
	FVector CenterLocation;
	FFloat16 Radius;

	bool HasYieldSignAlongRoad(float DistanceAlongLane) const;
	bool HasYieldSignThatRequiresStopAlongRoad(float DistanceAlongLane) const;
	bool HasYieldSignAtLaneStart() const;
	bool HasStopSignAtLaneStart() const;
	bool HasStopSignOrYieldSignAtLaneStart() const;
	bool HasTrafficLightAtLaneStart() const;
	bool HasTrafficSignThatRequiresStopAtLaneStart() const;

	float LaneLengthAtNextTrafficControl(float DistanceAlongLane) const;

	/** Clears all references to vehicles on this lane and reset all vehicle counters */  
	void ClearVehicles();

	/**
	 * Walks along the vehicles on this lane starting from TailVehicle and following the NextVehicle links,
	 * calling Function on each vehicle, until we reach a vehicle on another lane or we loop back to TailVehicle.
	 */
	void ForEachVehicleOnLane(const FMassEntityManager& EntityManager, FTrafficVehicleExecuteFunction Function) const;

	/** Space available for vehicle. */
	void ClearVehicleOccupancy();
	void RemoveVehicleOccupancy(const float SpaceToAdd);
	void AddVehicleOccupancy(const float SpaceToRemove);

	float SpaceAvailableFromStartOfLaneForVehicle(const FMassEntityManager& EntityManager, const bool bCheckLaneChangeGhostVehicles, const bool bCheckSplittingAndMergingGhostTailVehicles) const;

	bool TryGetDistanceFromStartOfLaneToTailVehicle(const FMassEntityManager& EntityManager, float& OutDistanceToTailVehicle) const;

	FORCEINLINE void IncrementNumVehiclesReadyToUseIntersectionLane()	// (See all READYLANE.)
	{
		++NumVehiclesReadyToUseIntersectionLane;
		ensureMsgf(NumVehiclesReadyToUseIntersectionLane <= MaxAllowedVehiclesReadyToUseIntersectionLane, TEXT("NumVehiclesReadyToUseIntersectionLane should never exceed MaxAllowedVehiclesReadyToUseIntersectionLane.  There is an error in the \"ready\" count logic.  Or, there is an unusually large number of (small?) vehicles ready to use this lane."));
	}
	
	FORCEINLINE void DecrementNumVehiclesReadyToUseIntersectionLane()	// (See all READYLANE.)
	{
		--NumVehiclesReadyToUseIntersectionLane;
		ensureMsgf(NumVehiclesReadyToUseIntersectionLane >= 0, TEXT("NumVehiclesReadyToUseIntersectionLane should never go below zero.  There is an error in the \"ready\" count logic."));
	}

	FORCEINLINE bool HasVehiclesReadyToUseIntersectionLane() const	// (See all READYLANE.)
	{
		ensureMsgf(NumVehiclesReadyToUseIntersectionLane >= 0, TEXT("NumVehiclesReadyToUseIntersectionLane should never go below zero.  There is an error in the \"ready\" count logic."));
		ensureMsgf(NumVehiclesReadyToUseIntersectionLane <= MaxAllowedVehiclesReadyToUseIntersectionLane, TEXT("NumVehiclesReadyToUseIntersectionLane should never exceed MaxAllowedVehiclesReadyToUseIntersectionLane.  There is an error in the \"ready\" count logic.  Or, there is an unusually large number of (small?) vehicles ready to use this lane."));
		
		return NumVehiclesReadyToUseIntersectionLane > 0;
	}

	FORCEINLINE void IncrementYieldingVehicles()
	{
		++NumYieldingVehicles;
		ensureMsgf(NumYieldingVehicles <= MaxAllowedYieldingVehiclesOnLane, TEXT("NumYieldingVehicles should never exceed MaxAllowedYieldingVehiclesOnLane.  There is an error in the yield count logic.  Or, there is an unusually large number of (small?) vehicles yielding on this lane."));
	}
	
	FORCEINLINE void DecrementYieldingVehicles()
	{
		--NumYieldingVehicles;
		ensureMsgf(NumYieldingVehicles >= 0, TEXT("NumYieldingVehicles should never go below zero.  There is an error in the yield count logic."));
	}

	FORCEINLINE bool HasYieldingVehicles() const
	{
		ensureMsgf(NumYieldingVehicles >= 0, TEXT("NumYieldingVehicles should never go below zero.  There is an error in the yield count logic."));
		ensureMsgf(NumYieldingVehicles <= MaxAllowedYieldingVehiclesOnLane, TEXT("NumYieldingVehicles should never exceed MaxAllowedYieldingVehiclesOnLane.  There is an error in the yield count logic.  Or, there is an unusually large number of (small?) vehicles yielding on this lane."));
		
		return NumYieldingVehicles > 0;
	}

	// Traffic density.

	/** NOTE - Usually between 0 and 1, but can be above 1 since SpaceAvailable may be negative. */
	FORCEINLINE float BasicDensity() const
	{
		return (Length - SpaceAvailable) / Length;
	}

	/** NOTE - Usually between 0 and 1, but can be above 1 since SpaceAvailable can be negative, and target density can be < 1. */
	FORCEINLINE float FunctionalDensity() const
	{
		static float MaxReturnValue = 100.0f;
		return MaxDensity > 0.0f ?
			FMath::Clamp(BasicDensity() / MaxDensity, 0.0f, MaxReturnValue) :
			MaxReturnValue;
	}
	
	FORCEINLINE float GetDownstreamFlowDensity() const
	{
		return DownstreamFlowDensity;
	}
		
	void UpdateDownstreamFlowDensity(float DownstreamFlowDensityMixtureFraction);

private:

	FFloat16 DownstreamFlowDensity = 0.0f;

	// This is a signed int8 on purpose.  The count should never be higher than the max of an int8.
	// And, we use the sign as a means to ensure the "reference counting" logic that governs
	// the NumVehiclesReadyToUseIntersectionLane field is working as intended.
	// That is, it should never go out of bounds.
	int8 NumVehiclesReadyToUseIntersectionLane = 0; // (See all READYLANE.)

	// Used to tighten the upper bound on NumVehiclesReadyToUseIntersectionLane.
	// If the number of vehicles ready to use an intersection lane exceeds this number,
	// we'll hit a bounds-checking ensure.
	static int8 MaxAllowedVehiclesReadyToUseIntersectionLane;	// (See all READYLANE.)

	// This is a signed int8 on purpose.  The count should never be higher than a few at most.
	// And, we use the sign as a means to ensure the "reference counting" logic that governs
	// the NumYieldingVehicles field is working as intended.  That is, it should never go out of bounds.
	int8 NumYieldingVehicles = 0;
	
	// Used to tighten the upper bound on NumYieldingVehicles.
	// If the number of vehicles yielding on this lane exceeds this number,
	// we'll hit a bounds-checking ensure.
	static int8 MaxAllowedYieldingVehiclesOnLane;
};

/**
 * Container for the traffic lane data associated to a specific registered ZoneGraph data.
 */
struct MASSTRAFFIC_API FMassTrafficZoneGraphData
{
	void Reset()
	{
		DataHandle.Reset();
		TrafficLaneDataArray.Reset();
		TrafficLaneDataLookup.Reset();
	}

	/* Handle of the storage the data was initialized from. */
	FZoneGraphDataHandle DataHandle;

	/* Runtime data for traffic lanes */ 
	TArray<FZoneGraphTrafficLaneData> TrafficLaneDataArray;

	/* ZoneGraph lane index -> TrafficLaneDataArray entry. Array size matches ZoneGraph storage */   
	TArray<FZoneGraphTrafficLaneData*> TrafficLaneDataLookup;

	FORCEINLINE const FZoneGraphTrafficLaneData* GetTrafficLaneData(const FZoneGraphLaneHandle LaneHandle) const
	{
		return TrafficLaneDataLookup[LaneHandle.Index];
	}
	
	FORCEINLINE const FZoneGraphTrafficLaneData* GetTrafficLaneData(const int32 LaneIndex) const
	{
		return TrafficLaneDataLookup[LaneIndex];
	}

	FORCEINLINE FZoneGraphTrafficLaneData* GetMutableTrafficLaneData(const FZoneGraphLaneHandle LaneHandle)
	{
		return TrafficLaneDataLookup[LaneHandle.Index];
	}
	
	FORCEINLINE FZoneGraphTrafficLaneData* GetMutableTrafficLaneData(const int32 LaneIndex)
	{
		return TrafficLaneDataLookup[LaneIndex];
	}
};

USTRUCT(BlueprintType)
struct MASSTRAFFIC_API FMassTrafficLanePriorityFilters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FZoneGraphTagFilter> LaneTagFilters;
};
