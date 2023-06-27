// Copyright Enguerran COBERT, All Rights Reserved.


#include "PMBumperBase.h"

//Unreal
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"

APMBumperBase::APMBumperBase()
{
	PrimaryActorTick.bCanEverTick = false;

	m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(m_root);

	m_boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	m_boxComponent->SetupAttachment(m_root);

	m_baseBumper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseBumper"));
	m_baseBumper->SetupAttachment(m_root);

	m_topBumper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopBumper"));
	m_topBumper->SetupAttachment(m_baseBumper);
}

void APMBumperBase::BeginPlay()
{
	Super::BeginPlay();

	BindBoxComponentEvent();
}

void APMBumperBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APMBumperBase::BindBoxComponentEvent()
{
	if (m_boxComponent)
	{
		m_boxComponent->OnComponentBeginOverlap.AddDynamic(this, &APMBumperBase::OnBoxComponentOverlapped);
	}
	else
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, box component is not constructed"), *GetName());
	}
}

void APMBumperBase::OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && m_validClassToBump.Contains(OtherActor->GetClass()))
	{
		UE_LOG(LogPlatformerPlugin, Warning, TEXT("Valid Bump"));

		if (ACharacter* lCharac = Cast<ACharacter>(OtherActor))
		{
			FVector lForceVector = OtherActor->GetActorUpVector() * m_bumperForce;

			lCharac->LaunchCharacter(lForceVector, false, true);

			return;
		}

		if (UPrimitiveComponent* lPrimitive = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent()))
		{
			FVector lForceVector = OtherActor->GetActorUpVector() * (m_bumperForce * m_bumperActorForceFactor);

			UE_LOG(LogPlatformerPlugin, Warning, TEXT("%f"), lPrimitive->GetPhysicsLinearVelocity().Z);

			if (lPrimitive->GetPhysicsLinearVelocity().Z != 0)
			{
				lPrimitive->SetPhysicsLinearVelocity(FVector::Zero());
			}

			UE_LOG(LogPlatformerPlugin, Warning, TEXT("%f"), lPrimitive->GetPhysicsLinearVelocity().Z);

			lPrimitive->AddImpulse(lForceVector);
		}
	}
}

