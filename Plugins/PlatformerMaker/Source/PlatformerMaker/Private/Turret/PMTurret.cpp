// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Turret/PMTurret.h"

//Unreal
#include "Perception/AIPerceptionComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"

APMTurret::APMTurret(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(m_root);

	m_perceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
}

void APMTurret::BeginPlay()
{
	Super::BeginPlay();

	ListenPerceptionComponentEvent();
}

void APMTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APMTurret::ListenPerceptionComponentEvent()
{
	if (IsValid(m_perceptionComponent))
	{
		m_perceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &APMTurret::OnPerceptionUpdate);
	}
}

void APMTurret::OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	//Check perception
	//Check valid actor
	//Check Valid Sight actor
	//Call On Sight

	if (Stimulus.WasSuccessfullySensed())
	{
		OnSight(Actor);
		StartLookAtTarget(Actor);
		StartShoot();
	}
	else
	{
		OnOutSight(Actor);
		StopLookAtTarget();
		StopShoot();
	}
}

void APMTurret::StartLookAtTarget(AActor* Target)
{
	if (IsValid(Target) && GetWorld())
	{
		if (UWorld* lWorld = GetWorld())
		{
			m_lookAtActor = Target;

			const float lDeltaTime = lWorld->GetDeltaSeconds();

			lWorld->GetTimerManager().SetTimer(m_lookAtTimerHandle, this, &APMTurret::LookAtTarget, lDeltaTime, true);
		}
	}
}

void APMTurret::StartShoot()
{
	if (UWorld* lWorld = GetWorld())
	{
		lWorld->GetTimerManager().SetTimer(m_shootTimerHandle, this, &APMTurret::Shoot, m_fireRate, true);
	}
}

void APMTurret::StopShoot()
{
	if (UWorld* lWorld = GetWorld())
	{
		lWorld->GetTimerManager().ClearTimer(m_shootTimerHandle);
	}
}

void APMTurret::StopLookAtTarget()
{
	if (UWorld* lWorld = GetWorld())
	{
		lWorld->GetTimerManager().ClearTimer(m_lookAtTimerHandle);

		m_lookAtActor = nullptr;
	}
}

void APMTurret::LookAtTarget()
{
	if (IsValid(m_lookAtActor))
	{
		m_lookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), m_lookAtActor->GetActorLocation());
		OnLookAtRotationComputed(m_lookAtRotation);
	}
}

void APMTurret::Shoot()
{
	OnShoot();
}

void APMTurret::OnSight(AActor* Actor)
{
	ReceiveOnSight(Actor);
}

void APMTurret::OnOutSight(AActor* Actor)
{
	ReceiveOnOutSight(Actor);
}

void APMTurret::OnLookAtRotationComputed(FRotator Rotator)
{
	ReceiveOnLookAtRotationComputed(Rotator);
}

void APMTurret::OnShoot()
{
	ReceiveOnShoot();
}

