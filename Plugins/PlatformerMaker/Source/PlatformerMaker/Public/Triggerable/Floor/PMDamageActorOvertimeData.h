#pragma once

#include "CoreMinimal.h"
#include "PMDamageActorOvertimeData.generated.h"

class AActor;

USTRUCT(BlueprintType, Blueprintable)
struct FPMDamageActorOvertimeData
{
	GENERATED_USTRUCT_BODY()

	/**************************** MEMBERS ******************************/
public:
	float CurrentTime = 0;
	AActor* Actor = nullptr;

	/**************************** FUNCTION ******************************/
public:

	void Reset()
	{
		CurrentTime = 0;
	}
};