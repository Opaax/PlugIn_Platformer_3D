// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "../Public/Triggerable/PMBumperBase.h"
#include "PlatformerMaker.h"

//Unreal
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"

APMBumperBase::APMBumperBase(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<UBoxComponent>(TriggerComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	m_baseBumper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseBumper"));
	m_baseBumper->SetupAttachment(m_root);

	m_topBumper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopBumper"));
	m_topBumper->SetupAttachment(m_baseBumper);

	//Add by default Character class
	m_validClassToTrigger.Add(ACharacter::StaticClass());
}

void APMBumperBase::BeginPlay()
{
	Super::BeginPlay();
}

void APMBumperBase::TriggerBeginPlay()
{
	Super::TriggerBeginPlay();
}

void APMBumperBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APMBumperBase::OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanBeTriggerBy(OtherActor))
	{
		UE_LOG(LogPlatformerPlugin, Warning, TEXT("Valid Bump"));

		if (ACharacter* lCharac = Cast<ACharacter>(OtherActor))
		{
			FVector lForceVector = OtherActor->GetActorUpVector() * m_bumperForce;

			lCharac->LaunchCharacter(lForceVector, false, true);

			OnBumped();

			return;
		}

		if (UPrimitiveComponent* lPrimitive = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent()))
		{
			FVector lForceVector = OtherActor->GetActorUpVector() * (m_bumperForce * m_bumperActorForceFactor);

			if (lPrimitive->GetPhysicsLinearVelocity().Z != 0)
			{
				lPrimitive->SetPhysicsLinearVelocity(FVector::Zero());
			}

			lPrimitive->AddImpulse(lForceVector);

			OnBumped();
		}
	}
}

void APMBumperBase::OnBumped()
{
	OnBumpedDelegate.Broadcast();

	ReceiveOnBumped();
}

