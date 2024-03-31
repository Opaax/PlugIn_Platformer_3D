// 2024 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "PMFindFloorResultStruct.generated.h"

/** Data about the floor for walking movement, used by CharacterMovementComponent. */
USTRUCT(BlueprintType)
struct PLATFORMERMAKER_API FPMFindFloorResult
{
	GENERATED_USTRUCT_BODY()

	/**
	* True if there was a blocking hit in the floor test that was NOT in initial penetration.
	* The HitResult can give more info about other circumstances.
	*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
	uint32 bBlockingHit : 1;

	/** True if the hit found a valid walkable floor. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
	uint32 bWalkableFloor : 1;

	/** True if the hit found a valid walkable floor using a line trace (rather than a sweep test, which happens when the sweep test fails to yield a walkable surface). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
	uint32 bLineTrace : 1;

	/** The distance to the floor, computed from the swept capsule trace. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
	float FloorDist;

	/** The distance to the floor, computed from the trace. Only valid if bLineTrace is true. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
	float LineDist;

	/** Hit result of the test that found a floor. Includes more specific data about the point of impact and surface normal at that point. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
	FHitResult HitResult;

public:

	FPMFindFloorResult()
		: bBlockingHit(false)
		, bWalkableFloor(false)
		, bLineTrace(false)
		, FloorDist(0.f)
		, LineDist(0.f)
		, HitResult(1.f)
	{
	}

	/** Returns true if the floor result hit a walkable surface. */
	bool IsWalkableFloor() const
	{
		return bBlockingHit && bWalkableFloor;
	}

	void Clear()
	{
		bBlockingHit = false;
		bWalkableFloor = false;
		bLineTrace = false;
		FloorDist = 0.f;
		LineDist = 0.f;
		HitResult.Reset(1.f, false);
	}

	/** Gets the distance to floor, either LineDist or FloorDist. */
	float GetDistanceToFloor() const
	{
		// When the floor distance is set using SetFromSweep, the LineDist value will be reset.
		// However, when SetLineFromTrace is used, there's no guarantee that FloorDist is set.
		return bLineTrace ? LineDist : FloorDist;
	}

	void SetFromSweep(const FHitResult& InHit, const float InSweepFloorDist, const bool bIsWalkableFloor);
	void SetFromLineTrace(const FHitResult& InHit, const float InSweepFloorDist, const float InLineDist, const bool bIsWalkableFloor);
};