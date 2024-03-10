// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/Data/PM_InputPlayerConfig.h"

UPM_InputPlayerConfig::UPM_InputPlayerConfig(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

const UInputAction* UPM_InputPlayerConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FDemoInputAction& lAction : NativeInputActions) {
		if (lAction.InputAction && (lAction.InputTag == InputTag)) {
			return lAction.InputAction;
		}
	}

	if (bLogNotFound) {
		UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UPM_InputPlayerConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FDemoInputAction& lAction : AbilityInputActions) {
		if (lAction.InputAction && (lAction.InputTag == InputTag)) {
			return lAction.InputAction;
		}
	}

	if (bLogNotFound) {
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
