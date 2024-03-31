// 2024 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMFindFloorResultStruct.h"
#include "PMStepDownResultStruct.generated.h"

/** Struct updated by StepUp() to return result of final step down, if applicable. */
struct PLATFORMERMAKER_API FPMStepDownResult
{
	GENERATED_USTRUCT_BODY()

	uint32 bComputedFloor : 1;		// True if the floor was computed as a result of the step down.
	FPMFindFloorResult FloorResult;	// The result of the floor test if the floor was updated.

	FPMStepDownResult()
		: bComputedFloor(false)
	{
	}
};