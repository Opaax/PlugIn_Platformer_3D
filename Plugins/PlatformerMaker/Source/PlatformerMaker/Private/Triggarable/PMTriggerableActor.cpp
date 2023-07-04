// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Triggerable/PMTriggerableActor.h"
#include "PlatformerMaker.h"


#include "Components/ShapeComponent.h"
#include "Components/SceneComponent.h"

//Do not change this
FName ATriggerableActor::TriggerComponentName(TEXT("TriggerComponent"));

/*
* Set default values
* Trigger component is a box by default
*/
ATriggerableActor::ATriggerableActor(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(m_root);

	m_triggerComponent = CreateDefaultSubobject<UShapeComponent>(TriggerComponentName);

	if (m_triggerComponent && m_root)
	{
		m_triggerComponent->SetupAttachment(m_root);
	}
}

bool ATriggerableActor::CanBeTriggerBy(AActor* OtherActor)
{
	return IsValid(OtherActor) && m_validClassToTrigger.Contains(OtherActor->GetClass());
}

void ATriggerableActor::BeginPlay()
{
	Super::BeginPlay();

	TriggerBeginPlay();
}

void ATriggerableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATriggerableActor::BindBoxComponentEvent()
{
	if (m_triggerComponent)
	{
		m_triggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ATriggerableActor::OnBoxComponentOverlapped);
	}
	else
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, trigger component is not constructed"), *GetName());
	}
}

void ATriggerableActor::TriggerBeginPlay()
{
	BindBoxComponentEvent();
}

void ATriggerableActor::OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanBeTriggerBy(OtherActor))
	{
		OnTrigger(OtherActor);
	}
}

void ATriggerableActor::OnTrigger(AActor* OtherTrigger)
{
	ReceiveOnTrigger(OtherTrigger);
}

