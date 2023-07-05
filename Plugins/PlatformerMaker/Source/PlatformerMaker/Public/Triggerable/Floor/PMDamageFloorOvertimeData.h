#pragma once

#include "CoreMinimal.h"
#include "PMDamageFloorOvertimeData.generated.h"

class AActor;

USTRUCT(BlueprintType, Blueprintable)
struct FPMDamageFloorOvertimeData
{
	GENERATED_USTRUCT_BODY()

	/**************************** MEMBERS ******************************/
public:

	float CurrentTime = 0;
	AActor* Actor = nullptr;

	/**************************** Function ******************************/
public:

	void Reset()
	{
		CurrentTime = 0;
	}
};