// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Demo/Data/PM_InputPlayerConfig.h"
#include "PM_InputComponentDemo.generated.h"


/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API UPM_InputComponentDemo : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
	/*---------------------------------- FUNCTIONS ----------------------------------*/
public:
	UPM_InputComponentDemo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UPM_InputPlayerConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UPM_InputPlayerConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
	/*---------------------------------- OVERRIDE ----------------------------------*/

};

template<class UserClass, typename FuncType>
void UPM_InputComponentDemo::BindNativeAction(const UPM_InputPlayerConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* lIA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(lIA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UPM_InputComponentDemo::BindAbilityActions(const UPM_InputPlayerConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FDemoInputAction& lAction : InputConfig->AbilityInputActions) {
		if (lAction.InputAction && lAction.InputTag.IsValid()) {
			if (PressedFunc) {
				BindHandles.Add(BindAction(lAction.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, lAction.InputTag).GetHandle());
			}

			if (ReleasedFunc) {
				BindHandles.Add(BindAction(lAction.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, lAction.InputTag).GetHandle());
			}
		}
	}
}