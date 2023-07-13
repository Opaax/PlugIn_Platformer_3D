// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Triggerable/PMDamageActor.h"

//Unreal
#include "Components/BoxComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

APMDamageActor::APMDamageActor(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<UBoxComponent>(TriggerComponentName))
{
}

void APMDamageActor::TriggerBeginPlay()
{
	Super::TriggerBeginPlay();
}

void APMDamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APMDamageActor::OnTrigger(AActor* OtherActor)
{
	Super::OnTrigger(OtherActor);

	DamageActor(OtherActor);
}

void APMDamageActor::DamageActor(AActor* DamageActor)
{
	//Apply damage on default damage system that UE provide
	const FDamageEvent lDamageEvent = FDamageEvent();
	DamageActor->TakeDamage(m_baseDamage, lDamageEvent, nullptr, this);

	OnActorDamage(DamageActor);
}

void APMDamageActor::OnActorDamage(AActor* DamageActor)
{
	ReceiveOnActorDamage(DamageActor);
}
