// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/PM_CharacterDemo.h"
#include "Demo/PM_PlayerControllerDemo.h"
#include "Demo/Components/PM_PlayableInputCompDemo.h"
#include "Utils/DebugMacro.h"
#include "Demo/Components/PM_AbilitySystemComponentDemo.h"
#include "Demo/Data/PM_PlayerPawnData.h"

//Unreal
#include "Components/CapsuleComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

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
	m_abilitySystemComp = CreateDefaultSubobject<UPM_AbilitySystemComponentDemo>(TEXT("AbilitySystemComp"));
}

void APM_CharacterDemo::BeginPlay()
{
	Super::BeginPlay();
}

void APM_CharacterDemo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

void APM_CharacterDemo::SetupConstraints()
{
	FVector lForward	= FVector::ForwardVector;
	FVector lUpward		= FVector::UpVector;

	if (GetController()) {
		if (GetController()->StartSpot != nullptr) {
			lForward = GetController()->StartSpot->GetActorForwardVector();
		}
	}
	else {
		lForward = GetActorRotation().RotateVector(lForward);
	}

	SetDirectionContraint(lForward, lUpward);
}

void APM_CharacterDemo::SetDirectionContraint(const FVector& ForwardConstraint, const FVector& UpConstraint)
{
	ensure(GetMovementComponent());

	GetMovementComponent()->SetPlaneConstraintEnabled(true);
	GetMovementComponent()->SetPlaneConstraintFromVectors(ForwardConstraint, UpConstraint);
}

void APM_CharacterDemo::ProcessInputForAbility(const float DeltaTime, const bool bGamePaused)
{
	if (GetAbilitySystemComp()) {
		GetAbilitySystemComp()->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
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

	PopulateAbilityComponentDefault();
}

void APM_CharacterDemo::UnPossessed()
{
	Super::UnPossessed();

	ClearAbilityComponent();
}

void APM_CharacterDemo::PawnClientRestart()
{
	Super::PawnClientRestart();

	CheckAutoAddMappingContext();
}

void APM_CharacterDemo::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void APM_CharacterDemo::SetPlayerDefaults()
{
	Super::SetPlayerDefaults();

	SetupConstraints();
}

void APM_CharacterDemo::TeleportSucceeded(bool bIsATest)
{
	//The parent character call OnTeleported localized In character movement, but if any custom movement do not implement this, lets go ahead here
	Super::TeleportSucceeded(bIsATest);

	SetupConstraints();
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

void APM_CharacterDemo::PopulateAbilityComponentDefault()
{
	if (GetAbilitySystemComp() && IsValid(m_pawnData) && !m_pawnData->GetAbilities().IsEmpty()) {
		for (auto& lAbilityData : m_pawnData->GetAbilities()) {
			GetAbilitySystemComp()->SetUpAbility(lAbilityData);
		}
	}
}

void APM_CharacterDemo::ClearAbilityComponent()
{
	if (GetAbilitySystemComp()) {
		GetAbilitySystemComp()->ClearAllAbilities();
	}
}
