// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Demo/PM_PlayerControllerDemo.h"
#include "Utils/DebugMacro.h"

//Unreal
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PM_PlayerControllerDemo)

DEFINE_LOG_CATEGORY_STATIC(LogPlayerControllerDemo, Log, All);

APM_PlayerControllerDemo::APM_PlayerControllerDemo(const FObjectInitializer& ObjectInitializer)
{
}

void APM_PlayerControllerDemo::AddInputMappingContext(const UInputMappingContext* InputMappingContext, const int32 Priority)
{
	if (!IsValid(InputMappingContext)) {
		DEBUG_WARNING_CUSTOM_CATEGORY(LogPlayerControllerDemo, TEXT("Trying to add NULL input mapping context"));
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* lSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		if (!lSubsystem->HasMappingContext(InputMappingContext)) {
			lSubsystem->AddMappingContext(InputMappingContext, Priority);
		}
		else {
			DEBUG_WARNING_CUSTOM_CATEGORY(LogPlayerControllerDemo, TEXT("Input Mapping Context: %s, is already added"), *InputMappingContext->GetName());
		}
	}
}

void APM_PlayerControllerDemo::RemoveInputMappingContext(const UInputMappingContext* InputMappingContext)
{
	if (!IsValid(InputMappingContext)) {
		DEBUG_WARNING_CUSTOM_CATEGORY(LogPlayerControllerDemo, TEXT("Trying to remove NULL input mapping context"));
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* lSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		if (!lSubsystem->HasMappingContext(InputMappingContext)) {
			lSubsystem->RemoveMappingContext(InputMappingContext);
		}
		else {
			DEBUG_WARNING_CUSTOM_CATEGORY(LogPlayerControllerDemo, TEXT("Input Mapping Context: %s, is not on the input mapping context list"), *InputMappingContext->GetName());
		}
	}

}