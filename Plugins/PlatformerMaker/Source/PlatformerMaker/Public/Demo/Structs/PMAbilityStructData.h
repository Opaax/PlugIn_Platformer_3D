#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "PMAbilityStructData.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct PLATFORMERMAKER_API FPMAbilityDataDemo 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category= "PMAbilityDataDemo" )
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly,Category= "PMAbilityDataDemo")
	int32 AbilityLevel;

	UPROPERTY(EditDefaultsOnly,Category= "PMAbilityDataDemo")
	FGameplayTag AbilityDynamicTag;

	FPMAbilityDataDemo():
		AbilityClass(UGameplayAbility::StaticClass()),
		AbilityLevel(1),
		AbilityDynamicTag()
	{}
};