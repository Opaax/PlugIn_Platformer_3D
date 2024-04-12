// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PM_AbilitySystemComponentDemo.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API UPM_AbilitySystemComponentDemo : public UAbilitySystemComponent
{
	GENERATED_UCLASS_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
protected:
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> m_inputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> m_inputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> m_inputHeldSpecHandles;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
public:
	static UPM_AbilitySystemComponentDemo* FindGetAbilityCompDemoInActor(AActor* const InCheckActor);

	UFUNCTION(BlueprintCallable)
	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable)
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	/*---------------------------------- OVERRIDE ----------------------------------*/

};
