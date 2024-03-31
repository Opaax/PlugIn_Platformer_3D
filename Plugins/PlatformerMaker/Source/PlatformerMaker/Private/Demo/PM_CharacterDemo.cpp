// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/PM_CharacterDemo.h"
#include "Demo/PM_PlayerControllerDemo.h"
#include "Demo/Components/PM_PlayableInputCompDemo.h"
#include "Utils/DebugMacro.h"

//Unreal
#include "Components/CapsuleComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "AbilitySystemComponent.h"

#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif //#if WITH_EDITORONLY_DATA

#include UE_INLINE_GENERATED_CPP_BY_NAME(PM_CharacterDemo)

DEFINE_LOG_CATEGORY_STATIC(LogCharacterDemo, Log, All);

APM_CharacterDemo::APM_CharacterDemo(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	m_playableInputComp = CreateDefaultSubobject<UPM_PlayableInputCompDemo>(TEXT("PlayableInputComp"));
	m_abilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
}

void APM_CharacterDemo::Input_Movement(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f) {
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f) {
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(MovementDirection, Value.Y);
	}
}

void APM_CharacterDemo::BeginPlay()
{
	Super::BeginPlay();
	
}

void APM_CharacterDemo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APM_CharacterDemo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APM_CharacterDemo::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	m_pmController = Cast<APM_PlayerControllerDemo>(NewController);
	
	if (m_pmController) {
		m_pmController->SetDemoCharacter(this);
	}
}

void APM_CharacterDemo::PawnClientRestart()
{
	Super::PawnClientRestart();

	CheckAutoAddMappingContext();
}

void APM_CharacterDemo::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	DEBUG_LOG(TEXT("Add Movement Input"));

	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void APM_CharacterDemo::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (IsValid(m_abilitySystemComp)) {
		m_abilitySystemComp->GetOwnedGameplayTags(TagContainer);
	}

	TagContainer = FGameplayTagContainer();
}

bool APM_CharacterDemo::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (IsValid(m_abilitySystemComp)) {
		return m_abilitySystemComp->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool APM_CharacterDemo::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (IsValid(m_abilitySystemComp)) {
		return m_abilitySystemComp->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool APM_CharacterDemo::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (IsValid(m_abilitySystemComp)) {
		return m_abilitySystemComp->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

UAbilitySystemComponent* APM_CharacterDemo::GetAbilitySystemComponent() const
{
	return GetAbilitySystemComp();
}

void APM_CharacterDemo::CheckAutoAddMappingContext()
{
	check(m_playableInputComp);

	m_playableInputComp->InitializePlayerInput(m_pmController);
}
