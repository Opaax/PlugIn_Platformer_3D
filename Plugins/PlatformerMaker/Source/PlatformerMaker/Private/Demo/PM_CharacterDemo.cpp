// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/PM_CharacterDemo.h"
#include "Demo/PM_PlayerControllerDemo.h"

//Unreal
#include "Components/CapsuleComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"

#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif //#if WITH_EDITORONLY_DATA

#include UE_INLINE_GENERATED_CPP_BY_NAME(PM_CharacterDemo)

DEFINE_LOG_CATEGORY_STATIC(LogCharacterDemo, Log, All);

FName APM_CharacterDemo::CapsuleComponentName(TEXT("CapsuleComponent"));

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

	m_defaultInputMappingContextPriority = 0;
	m_defaultInputMappingContext = nullptr;
	bAutoAddMappingContext = true;

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

	CheckAutoAddMappingContext();
}

void APM_CharacterDemo::CheckAutoAddMappingContext()
{
	if (bAutoAddMappingContext && IsValid(m_pmController)) {
		m_pmController->AddInputMappingContext(m_defaultInputMappingContext, m_defaultInputMappingContextPriority);
	}
}
