// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Demo/Structs/PMAbilityStructData.h"
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

	UFUNCTION(BlueprintCallable, Category = "DemoAbilityComp")
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	UFUNCTION(BlueprintCallable, Category = "DemoAbilityComp")
	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable, Category = "DemoAbilityComp")
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable, Category = "DemoAbilityComp")
	void SetUpAbility(const FPMAbilityDataDemo& InAbilityData);
	/*---------------------------------- OVERRIDE ----------------------------------*/

};
