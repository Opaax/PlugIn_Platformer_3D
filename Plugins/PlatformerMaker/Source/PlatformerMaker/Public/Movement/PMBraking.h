#pragma once

#include "CoreMinimal.h"
#include "PMBraking.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct PLATFORMERMAKER_API FPMBraking
{
	GENERATED_USTRUCT_BODY()

	/*---------------------------------- MEMBERS ----------------------------------*/
public:
	UPROPERTY(Category = "Braking", EditDefaultsOnly, BlueprintReadOnly)
	float BrakingFriction;

	UPROPERTY(Category = "Braking", EditDefaultsOnly, BlueprintReadOnly)
	float BrakingDeceleration;

	UPROPERTY(Category = "Braking", EditDefaultsOnly, BlueprintReadOnly)
	float BrakingFrictionFactor;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
public:
	FPMBraking() :
		BrakingFriction(1),
		BrakingDeceleration(500.F),
		BrakingFrictionFactor(1)
	{
	}

	FPMBraking(const float InBrakingFriction, const float InBrakingDeceleration, const float InBrakingFrictionFactor) :
		BrakingFriction(InBrakingFriction),
		BrakingDeceleration(InBrakingDeceleration),
		BrakingFrictionFactor(InBrakingFrictionFactor)
	{}

	/*---------------------------------- OVERRIDE ----------------------------------*/
};