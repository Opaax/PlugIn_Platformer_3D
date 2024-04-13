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

void UPM_AbilitySystemComponentDemo::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//TODO Custom Ability setup as Lyra to make possibility to have ability with input hold

	//
	// Process all abilities that activate when the input is held.
	//
	for (const FGameplayAbilitySpecHandle& lSpecHandle : m_inputHeldSpecHandles) {
		if (const FGameplayAbilitySpec* lAbilitySpec = FindAbilitySpecFromHandle(lSpecHandle)) {
			if (lAbilitySpec->Ability && !lAbilitySpec->IsActive())
			{
				const UGameplayAbility* lAbilityCDO = CastChecked<UGameplayAbility>(lAbilitySpec->Ability);

				AbilitiesToActivate.AddUnique(lAbilitySpec->Handle);
			}
		}
	}

	//
	// Process all abilities that had their input pressed this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : m_inputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UGameplayAbility* lAbilityCDO = CastChecked<UGameplayAbility>(AbilitySpec->Ability);

					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	//
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	//
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	//
	// Process all abilities that had their input released this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : m_inputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//
	// Clear the cached ability handles.
	//
	m_inputPressedSpecHandles.Reset();
	m_inputReleasedSpecHandles.Reset();
}

void UPM_AbilitySystemComponentDemo::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid()) {
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items) {
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))) {
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
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))){
				m_inputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				m_inputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UPM_AbilitySystemComponentDemo::SetUpAbility(const FPMAbilityDataDemo& InAbilityData)
{
	if (IsValid(InAbilityData.AbilityClass) && InAbilityData.AbilityDynamicTag.IsValid()) {
		UGameplayAbility* lAbilityCDO = InAbilityData.AbilityClass->GetDefaultObject<UGameplayAbility>();

		FGameplayAbilitySpec lSpec(lAbilityCDO, InAbilityData.AbilityLevel);
		lSpec.SourceObject = GetOwnerActor();
		lSpec.DynamicAbilityTags.AddTag(InAbilityData.AbilityDynamicTag);

		const FGameplayAbilitySpecHandle lAbilitySpecHandle = GiveAbility(lSpec);

		lAbilitySpecHandle.IsValid();
	}
}