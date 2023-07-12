// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Turret/PMTurret.h"
#include "Turret/PMBullet.h"
#include "PlatformerMaker.h"

//Unreal
#include "Perception/AIPerceptionComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"

APMTurret::APMTurret(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(m_root);

	m_spawnBulletPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	//m_spawnBulletPoint->SetupAttachment(m_root);

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
	//Check if the Stimulus is positif
	// Success mean 'a new actor stimulate the component'
	if (Stimulus.WasSuccessfullySensed())
	{
		//Add the new actor on the on sight list
		AddActorOnSightList(Actor);

		//Call back for bp
		OnSight(Actor);

		//Start time functions
		StartLookAtTarget(Actor);
		StartShoot();
	}
	else
	{
		//Remove actor from on sight list
		RemoveActorOnSightList(Actor);

		//Callback for bp
		OnOutSight(Actor);

		//Find out new target to look at if any
		//Otherwise Look at target set to null
		CheckForNewPriorityLookAtTarget();

		//If there is no more look at actors
		//Stop behavior, Stop time functions
		if (!m_lookAtActor)
		{
			StopLookAtTarget();
			StopShoot();
		}
	}
}

void APMTurret::CheckPriorityLookAtTarget_Implementation(AActor* NewActor)
{
	//For the base plugin,
	//Set the target only if the target is null
	if (!m_lookAtActor && NewActor)
	{
		m_lookAtActor = NewActor;
	}
}

void APMTurret::CheckForNewPriorityLookAtTarget_Implementation()
{
	//For the base plugin
	//Set the first element of the list
	if (m_actorsOnSight.Num() > 0)
	{
		m_lookAtActor = m_actorsOnSight[0];
	}
	else
	{
		m_lookAtActor = nullptr;
	}
}

void APMTurret::StartLookAtTarget(AActor* Target)
{
	if (IsValid(Target))
	{
		if (UWorld* lWorld = GetWorld())
		{
			CheckPriorityLookAtTarget(Target);

			const float lDeltaTime = lWorld->GetDeltaSeconds();

			lWorld->GetTimerManager().SetTimer(m_lookAtTimerHandle, this, &APMTurret::LookAtTarget, lDeltaTime, true);
		}
		else
		{
			UE_LOG(LogPlatformerPlugin, Warning, TEXT("%s, World not valid"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogPlatformerPlugin, Warning, TEXT("%s, Start look target, but target is not valid"), *GetName());
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
	SpawnBullet();

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

void APMTurret::SpawnBullet()
{
	if (UWorld* lWorld = GetWorld())
	{
		if (m_spawnBulletPoint)
		{
			const FTransform& lTrans = 
				FTransform(
					m_spawnBulletPoint->GetComponentTransform().GetRotation(), 
					m_spawnBulletPoint->GetComponentTransform().GetLocation(), 
					FVector(1,1,1) //avoid weird stuff while inherited/reorder in blueprint
				);

			FActorSpawnParameters lParams = FActorSpawnParameters();

			lParams.Owner = this;

			AActor* lBullet = lWorld->SpawnActor<AActor>(m_bulletClass, lTrans, lParams);
		}
	}
}

void APMTurret::AddActorOnSightList_Implementation(AActor* NewActor)
{
	if (!NewActor)
	{
		UE_LOG(LogPlatformerPlugin, Warning, TEXT("%s, You looking to add actor on sight list. However, this actor is null"), *GetName());
		return;
	}

	if (m_actorsOnSight.IsEmpty())
	{
		m_actorsOnSight.Add(NewActor);
	}
	else
	{
		if (!m_actorsOnSight.Contains(NewActor))
		{
			m_actorsOnSight.Add(NewActor);
		}
		else
		{
			UE_LOG(LogPlatformerPlugin, Warning, TEXT("%s, You looking to add actor on sight list. However, this actor already on the list. Make sure to remove it if out of sight"), *GetName());
		}
	}
}

void APMTurret::RemoveActorOnSightList_Implementation(AActor* RemoveActor)
{
	if (!RemoveActor)
	{
		UE_LOG(LogPlatformerPlugin, Warning, TEXT("%s, You looking to remove actor from sight list. However, this actor is null"), *GetName());
		return;
	}

	if (m_actorsOnSight.IsEmpty())
	{
		UE_LOG(LogPlatformerPlugin, Warning, TEXT("%s, You looking to remove actor from sight list. However, the list is empty. Make sure to remove it if out of sight"), *GetName());

	}
	else
	{
		if (m_actorsOnSight.Contains(RemoveActor))
		{
			m_actorsOnSight.Remove(RemoveActor);
		}
		else
		{
			UE_LOG(LogPlatformerPlugin, Warning, TEXT("%s, You looking to remove actor from sight list. However, the actor are not on the list. Make sure to add it if on sight"), *GetName());
		}
	}
}
