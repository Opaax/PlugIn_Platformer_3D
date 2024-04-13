#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "PMAbilityStructData.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct PLATFORMERMAKER_API FPMAbilityDataDemo 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AbilityDynamicTag;

	FPMAbilityDataDemo():
		AbilityClass(UGameplayAbility::StaticClass()),
		AbilityLevel(1),
		AbilityDynamicTag()
	{}
};