// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/Components/PM_PlayableInputCompDemo.h"
#include "Demo/PM_PlayerControllerDemo.h"
#include "Demo/Components/PM_InputComponentDemo.h"
#include "Utils/DebugMacro.h"
#include "Demo/PM_CharacterDemo.h"
#include "Demo/FDemoTags.h"

#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

//Unreal Forward
class ULocalPlayer;

void UPM_PlayableInputCompDemo::Input_Movement_Internal(const FInputActionValue& InputActionValue)
{
	if (!IsValid(m_demoCharacter)) {
		DEBUG_ERROR(TEXT("[%s] is owner is not a demo character"), *GetNameSafe(this));
		return;
	}

	m_demoCharacter->Input_Movement(InputActionValue);
}

UPM_PlayableInputCompDemo::UPM_PlayableInputCompDemo(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	m_demoCharacter = nullptr;
}

void UPM_PlayableInputCompDemo::BeginPlay()
{
	Super::BeginPlay();

	m_demoCharacter = GetPawn<APM_CharacterDemo>();

	if (!IsValid(m_demoCharacter)) {
		DEBUG_ERROR(TEXT("Not Demo character"));
	}
}

void UPM_PlayableInputCompDemo::InitializePlayerInput_Implementation(APM_PlayerControllerDemo* PlayerControllerDemo)
{
	DEBUG_LOG(TEXT("%s,Start init player input"), *GetNameSafe(this));

	check(PlayerControllerDemo);

	APM_CharacterDemo* lCharac = PlayerControllerDemo->GetDemoCharacter();
	check(lCharac);

	const ULocalPlayer* lLocalPlayer = PlayerControllerDemo->GetLocalPlayer();
	check(lLocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* lSubsystem = lLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(lSubsystem);

	lSubsystem->ClearAllMappings();

	UInputMappingContext* lIMC = m_defaultMappingContext.SoftMappingContext.Get();

	if (!IsValid(lIMC)) {
		m_defaultMappingContext.SoftMappingContext.LoadSynchronous();
		lIMC = m_defaultMappingContext.SoftMappingContext.Get();
	}

	if (IsValid(lIMC)) {
		
		DEBUG_LOG(TEXT("%s, add mapping context"), *GetNameSafe(this));

		lSubsystem->AddMappingContext(lIMC, m_defaultMappingContext.InputPriority);

		UPM_InputComponentDemo* lDemoInputComp = Cast<UPM_InputComponentDemo>(lCharac->InputComponent);
		check(lDemoInputComp);
		
		if (!IsValid(m_inputConfig)) {
			DEBUG_ERROR(TEXT("[%s] Input Config is not set correctly"), *GetNameSafe(this));
			return;
		}

		if (!IsValid(m_demoCharacter)) {

			m_demoCharacter = GetPawn<APM_CharacterDemo>();

			if (!IsValid(m_demoCharacter)) {
				DEBUG_ERROR(TEXT("[%s] owner, is not a demo character"), *GetNameSafe(this));
				return;
			}
		}

		lDemoInputComp->BindNativeAction(m_inputConfig, DemoTags::Input_Native_Move.GetTag(), ETriggerEvent::Triggered, this, &ThisClass::Input_Movement_Internal, false);
		//TODO bind native & default ability such as jump
		//LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

	}
	else {
		DEBUG_WARNING(TEXT("%s, add mapping context fail"), *GetNameSafe(this));
	}
}