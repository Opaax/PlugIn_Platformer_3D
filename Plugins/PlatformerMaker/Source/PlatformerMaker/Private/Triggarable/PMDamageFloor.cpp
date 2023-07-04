// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Triggerable/PMDamageFloor.h"

//Unreal
#include "Components/BoxComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

APMDamageFloor::APMDamageFloor(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<UBoxComponent>(TriggerComponentName))
{
}

void APMDamageFloor::TriggerBeginPlay()
{
	Super::TriggerBeginPlay();
}

void APMDamageFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APMDamageFloor::OnTrigger(AActor* OtherActor)
{
	Super::OnTrigger(OtherActor);

	//Apply damage on default damage system that UE provide
	const FDamageEvent lDamageEvent = FDamageEvent();
	OtherActor->TakeDamage(m_baseDamage, lDamageEvent, nullptr, this);
}
