// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../Structs/PMAbilityStructData.h"
#include "PM_PlayerPawnData.generated.h"

/**
 * Data that player pawn will gether when posses
 */
UCLASS()
class PLATFORMERMAKER_API UPM_PlayerPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
private:
	UPROPERTY(EditDefaultsOnly, Category = "Ability",BlueprintGetter = "GetAbilities", meta = (DisplayName = "Abilities"))
	TArray<FPMAbilityDataDemo> m_abilities;
	/*---------------------------------- FUNCTION ----------------------------------*/
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category= "PlayerPawnData")
	TArray<FPMAbilityDataDemo> GetAbilities() const { return m_abilities; }

	UFUNCTION(BlueprintCallable, Category= "PlayerPawnData")
	void GetAbilitiesRef(TArray<FPMAbilityDataDemo>& OutAbility) { OutAbility = m_abilities; }

	/*---------------------------------- OVERRIDE ----------------------------------*/
};
