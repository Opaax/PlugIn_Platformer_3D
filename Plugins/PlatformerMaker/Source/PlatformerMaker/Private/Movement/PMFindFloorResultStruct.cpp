// 2024 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "PMFindFloorResultStruct.h"

void FPMFindFloorResult::SetFromLineTrace(const FHitResult& InHit, const float InSweepFloorDist, const float InLineDist, const bool bIsWalkableFloor)
{
	// We require a sweep that hit if we are going to use a line result.
	check(HitResult.bBlockingHit);
	if (HitResult.bBlockingHit && InHit.bBlockingHit)
	{
		// Override most of the sweep result with the line result, but save some values
		FHitResult OldHit(HitResult);
		HitResult = InHit;

		// Restore some of the old values. We want the new normals and hit actor, however.
		HitResult.Time = OldHit.Time;
		HitResult.ImpactPoint = OldHit.ImpactPoint;
		HitResult.Location = OldHit.Location;
		HitResult.TraceStart = OldHit.TraceStart;
		HitResult.TraceEnd = OldHit.TraceEnd;

		bLineTrace = true;
		FloorDist = InSweepFloorDist;
		LineDist = InLineDist;
		bWalkableFloor = bIsWalkableFloor;
	}
}

void FPMFindFloorResult::SetFromSweep(const FHitResult& InHit, const float InSweepFloorDist, const bool bIsWalkableFloor)
{
	bBlockingHit = InHit.IsValidBlockingHit();
	bWalkableFloor = bIsWalkableFloor;
	bLineTrace = false;
	FloorDist = InSweepFloorDist;
	LineDist = 0.f;
	HitResult = InHit;
}
