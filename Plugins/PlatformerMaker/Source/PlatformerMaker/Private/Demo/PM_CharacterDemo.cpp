// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/PM_CharacterDemo.h"
#include "Demo/PM_PlayerControllerDemo.h"
#include "Demo/Components/PM_PlayableInputCompDemo.h"
#include "Demo/Components/PM_CharacterMovementDemo.h"
#include "Utils/DebugMacro.h"
#include "Movement/PMCharacterMovement.h"

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

FName APM_CharacterDemo::CapsuleComponentName(TEXT("CapsuleComponent"));
FName APM_CharacterDemo::CharacterMovementComponentName(TEXT("CharacterMovement"));

APM_CharacterDemo::APM_CharacterDemo(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	m_capsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(CapsuleComponentName);
	m_capsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	m_capsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	m_capsuleComponent->CanCharacterStepUpOn = ECB_No;
	m_capsuleComponent->SetShouldUpdatePhysicsVolume(true);
	m_capsuleComponent->SetCanEverAffectNavigation(false);
	m_capsuleComponent->bDynamicObstacle = true;
	RootComponent = m_capsuleComponent;

	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	m_playableInputComp = CreateDefaultSubobject<UPM_PlayableInputCompDemo>(TEXT("PlayableInputComp"));
	m_abilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	m_characterMovement = CreateDefaultSubobject<UPMCharacterMovement>(CharacterMovementComponentName);
	
	// Structure to hold one-time initialization
	struct FConstructorStaticsDemoCharacter
	{
		FName ID_Characters;
		FText NAME_Characters;
		FConstructorStaticsDemoCharacter()
			: ID_Characters(TEXT("DemoCharacters"))
			, NAME_Characters(NSLOCTEXT("SpriteCategory", "DemoCharacters", "DemoCharacters"))
		{
		}
	};
	static FConstructorStaticsDemoCharacter ConstructorStatics;

#if WITH_EDITORONLY_DATA
	m_arrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (m_arrowComponent)
	{
		m_arrowComponent->ArrowColor = FColor(255, 200, 255);
		m_arrowComponent->ArrowSize = 2.f;
		m_arrowComponent->bTreatAsASprite = true;
		m_arrowComponent->SpriteInfo.Category = ConstructorStatics.ID_Characters;
		m_arrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Characters;
		m_arrowComponent->SetupAttachment(m_capsuleComponent);
		m_arrowComponent->bIsScreenSizeScaled = true;
		m_arrowComponent->SetSimulatePhysics(false);
	}
#endif // WITH_EDITORONLY_DATA
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
	
	CharacterMovement()->SetMovementTag(GetMovementWalkTag());
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
	
	if (!m_pmController) {
		DEBUG_ERROR(TEXT("[%s] New controller is not a [%s]"), *GetNameSafe(this), *GetNameSafe(APM_PlayerControllerDemo::StaticClass()));
		return;
	}

	m_pmController->SetDemoCharacter(this);
	CharacterMovement()->SetMovementTag(GetMovementWalkTag());
}

void APM_CharacterDemo::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (!m_pmController) {
		return;
	}

	CheckAutoAddMappingContext();
}

void APM_CharacterDemo::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	DEBUG_LOG(TEXT("Add Movement Input"));

	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

UPawnMovementComponent* APM_CharacterDemo::GetMovementComponent() const
{
	return m_characterMovement;
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
