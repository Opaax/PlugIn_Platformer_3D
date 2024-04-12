// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/Components/PM_AbilitySystemComponentDemo.h"

UPM_AbilitySystemComponentDemo::UPM_AbilitySystemComponentDemo(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	//TODO 
	//Process Input 
	//PlayerController DEMO Process Input -> Update here too
}

UPM_AbilitySystemComponentDemo* UPM_AbilitySystemComponentDemo::FindGetAbilityCompDemoInActor(AActor* const InCheckActor)
{
	if (IsValid(InCheckActor)) {
		return InCheckActor->FindComponentByClass<UPM_AbilitySystemComponentDemo>();
	}
	return nullptr;
}

void UPM_AbilitySystemComponentDemo::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid()) {
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items) {
			if (AbilitySpec.Ability /*&& (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))*/) {
				m_inputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				m_inputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UPM_AbilitySystemComponentDemo::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid()){
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items){
			if (AbilitySpec.Ability /*&& (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))*/){
				m_inputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				m_inputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}
