// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Demo/PM_PlayerControllerDemo.h"
#include "Utils/DebugMacro.h"
#include "Demo/PM_CharacterDemo.h"
#include "Demo/PM_PlayerCameraManagerDemo.h"

//Unreal
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PM_PlayerControllerDemo)

DEFINE_LOG_CATEGORY_STATIC(LogPlayerControllerDemo, Log, All);

APM_PlayerControllerDemo::APM_PlayerControllerDemo(const FObjectInitializer& ObjectInitializer)
{
	bAutoManageActiveCameraTarget = false;
	PlayerCameraManagerClass = APM_PlayerCameraManagerDemo::StaticClass();
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

void APM_PlayerControllerDemo::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	bool lbHasCamera = false;

	if (aPawn) {
		if (APM_CharacterDemo* lDemoCharacter = Cast<APM_CharacterDemo>(aPawn)) {
			if (m_demoCamManager) {
				m_demoCamManager->SpawnCameraDemo(aPawn);
				lbHasCamera = true;
			}
		}
	}

	if (!lbHasCamera) {
		SetViewTarget(aPawn);
	}
}

void APM_PlayerControllerDemo::OnUnPossess()
{
	Super::OnUnPossess();

	if (m_demoCamManager) {
		m_demoCamManager->DestroyCameraDemo();
	}
}

void APM_PlayerControllerDemo::SpawnPlayerCameraManager()
{
	Super::SpawnPlayerCameraManager();

	m_demoCamManager = CastChecked<APM_PlayerCameraManagerDemo>(PlayerCameraManager, ECastCheckedType::NullAllowed);
}

void APM_PlayerControllerDemo::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	if (m_demoCharacter) {
		m_demoCharacter->ProcessInputForAbility(DeltaTime, bGamePaused);
	}
}