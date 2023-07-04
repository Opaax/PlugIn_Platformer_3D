// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Platform/PMTransporterPlatform.h"
#include "PlatformerMaker.h"
#include "../../Public/Components/PMInterpToMovementComponent.h"

//Unreal
#include "Components/BoxComponent.h"
#include "Components/InterpToMovementComponent.h"
#include "GameFramework/Character.h"


APMTransporterPlatform::APMTransporterPlatform(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	m_triggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	m_triggerComponent->SetupAttachment(m_root);
}

void APMTransporterPlatform::BeginPlay()
{
	Super::BeginPlay();

	//Disable Platform tick
	EnableMovingPlatform(false);

	//Listen Component overlap event
	BindBoxComponentEvent();
}

void APMTransporterPlatform::OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!m_interpToMovementComponent->IsComponentTickEnabled())
	{
		if (CanTrigger(OtherActor))
		{
			//Set the interp type
			if (bIsOnStart)
			{
				m_interpToMovementComponent->BehaviourType = EInterpToBehaviourType::OneShot;
				m_interpToMovementComponent->RestartForNormal();
			}
			else
			{
				m_interpToMovementComponent->BehaviourType = EInterpToBehaviourType::OneShot_Reverse;
				m_interpToMovementComponent->RestartForReverse();
			}

			if(!m_interpToMovementComponent->OnInterpToStop.Contains(this, FName("OnInterpComponentStop")))
				m_interpToMovementComponent->OnInterpToStop.AddDynamic(this, &APMTransporterPlatform::OnInterpComponentStop);

			OnPlatformTrigger();
		}
	}
}

void APMTransporterPlatform::OnPlatformTrigger()
{
	EnableMovingPlatform(true);

	ReceiveOnPlatformTrigger();
}

bool APMTransporterPlatform::CanTrigger(AActor* OtherActor)
{
	if (ACharacter* lCharacter = Cast<ACharacter>(OtherActor))
	{
		return IsValid(OtherActor) && m_validCharacterClassToTrigger.Contains(lCharacter->GetClass());
	}

	return false;
}

void APMTransporterPlatform::OnInterpComponentStop(const FHitResult& ImpactResult, float Time)
{
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), Time));

	EnableMovingPlatform(false);

	if (Time > .9f)
	{
		bIsOnStart = false;
	}
	else
	{
		bIsOnStart = true;
	}
}

void APMTransporterPlatform::EnableMovingPlatform(bool NewEnable)
{
	//By default disable tick
	if (IsValid(m_interpToMovementComponent))
		m_interpToMovementComponent->SetComponentTickEnabled(NewEnable);
	else
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, The Interp component is not valid"), *GetNameSafe(this));
	}
}

void APMTransporterPlatform::BindBoxComponentEvent()
{
	if (m_triggerComponent)
	{
		m_triggerComponent->OnComponentBeginOverlap.AddDynamic(this, &APMTransporterPlatform::OnBoxComponentOverlapped);
	}
	else
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, trigger component is not constructed"), *GetName());
	}
}
