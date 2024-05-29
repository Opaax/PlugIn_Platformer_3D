// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Demo/PM_PlayerControllerDemo.h"
#include "Utils/DebugMacro.h"
#include "Demo/PM_CharacterDemo.h"
#include "Demo/PM_PlayerCameraManagerDemo.h"
#include "Demo/PM_GameModeDemo.h"
#include "Demo/PM_HUDDemo.h"
#include "Demo/PM_DemoCoreDelegates.h"
#include "SubSystem/PMCheckpointSubsytem.h"

//Unreal
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerStart.h"

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

void APM_PlayerControllerDemo::SetDemoCharacter(APM_CharacterDemo* InCharacter)
{
	if (m_demoCharacter == nullptr || m_demoCharacter != InCharacter) {
		m_demoCharacter = InCharacter;
	}


	if (m_demoCharacter) {
		if (MyHUD) {
			APM_HUDDemo* lDemoHUD = GetHUD<APM_HUDDemo>();

			if (lDemoHUD) {
				lDemoHUD->SetLifeComponent(m_demoCharacter->GetLifeComp());
			}
		}
		else {
			//In case the pawn is already on map.
			//The pawn will set itself on controller, but te HUD could be null at this moment.
			//make the HUD spawn then recall set Char
			SpawnDefaultHUD();
			SetDemoCharacter(m_demoCharacter);
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

			if (!lbHasCamera) {
				SetViewTarget(aPawn);
			}

			if (!IsValid(MyHUD)) {
				SpawnDefaultHUD();
				SetDemoCharacter(lDemoCharacter);
			}
		}
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

void APM_PlayerControllerDemo::PawnPendingDestroy(APawn* inPawn)
{
	bool lbShouldRestartPawn = (m_demoCharacter && inPawn) && (inPawn == m_demoCharacter);

	Super::PawnPendingDestroy(inPawn);

	if (lbShouldRestartPawn) {
		bool lbIsOnAnim = false;

		if (MyHUD && MyHUD->IsA<APM_HUDDemo>()) {
			APM_HUDDemo* lHUD = CastChecked<APM_HUDDemo>(MyHUD, ECastCheckedType::NullAllowed);
			
			if (lHUD) {
				FDeathTransitionCallback lTransitionCallback;

				lTransitionCallback.BindUFunction(this, FName("ProcessRestartDemo"));

				lHUD->LaunchDeathTransition(3, lTransitionCallback);

				lbIsOnAnim = true;
			}
		}

		if (!lbIsOnAnim) {
			ProcessRestartDemo();
		}
	}
}

void APM_PlayerControllerDemo::SpawnDefaultHUD()
{
	Super::SpawnDefaultHUD();

	DEBUG_LOG_SCREEN(-1, 4.f, FColor::Yellow, TEXT("Auto spawn HUD"));
}

void APM_PlayerControllerDemo::DestroyHUD()
{
	if (IsValid(MyHUD)) {
		MyHUD->Destroy();
	}
}

void APM_PlayerControllerDemo::ProcessRestartDemo()
{
	if (GetWorld()) {
		FlushPressedKeys();

		UPMCheckpointSubsytem* lCheckPointSubsystem = GetWorld()->GetSubsystem<UPMCheckpointSubsytem>();

		if (lCheckPointSubsystem) {
			if (lCheckPointSubsystem->GetCurrentCheckPoint() && lCheckPointSubsystem->GetCurrentCheckPointPlayerStart()) {
				GetWorld()->GetAuthGameMode()->RestartPlayerAtPlayerStart(this, lCheckPointSubsystem->GetCurrentCheckPointPlayerStart());
				return;
			}
		}

		GetWorld()->GetAuthGameMode()->RestartPlayer(this);
	}
}