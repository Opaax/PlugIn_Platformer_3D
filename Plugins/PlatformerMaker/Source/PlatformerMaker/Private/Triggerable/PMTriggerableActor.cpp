// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Triggerable/PMTriggerableActor.h"
#include "PlatformerMaker.h"

//Unreal
#include "Components/ShapeComponent.h"
#include "Components/SceneComponent.h"

//Do not change this
FName APMTriggerableActor::TriggerComponentName(TEXT("TriggerComponent"));

/*
* Set default values
*/
APMTriggerableActor::APMTriggerableActor(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
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

void APMTriggerableActor::BeginPlay()
{
	Super::BeginPlay();

	TriggerBeginPlay();
}

void APMTriggerableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APMTriggerableActor::CanBeTriggerBy(AActor* OtherActor)
{
	return IsValid(OtherActor) && m_validClassToTrigger.Contains(OtherActor->GetClass());
}

void APMTriggerableActor::BindTriggerComponentEvent()
{
	if (m_triggerComponent)
	{
		m_triggerComponent->OnComponentBeginOverlap.AddDynamic(this, &APMTriggerableActor::OnTriggerComponentOverlapped);
		m_triggerComponent->OnComponentEndOverlap.AddDynamic(this, &APMTriggerableActor::OnTriggerComponentEndOverlapped);
	}
	else
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, trigger component is not constructed"), *GetName());
	}
}

void APMTriggerableActor::RemoveBindTriggerComponentEvent()
{
	if (m_triggerComponent != nullptr)
	{
		if(m_triggerComponent->OnComponentBeginOverlap.IsBound())
			m_triggerComponent->OnComponentBeginOverlap.RemoveAll(this);

		if(m_triggerComponent->OnComponentEndOverlap.IsBound())
			m_triggerComponent->OnComponentEndOverlap.RemoveAll(this);
	}
	else
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, trigger component is not constructed"), *GetName());
	}
}

void APMTriggerableActor::TriggerBeginPlay()
{
	BindTriggerComponentEvent();
}

void APMTriggerableActor::OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanBeTriggerBy(OtherActor))
	{
		OnTrigger(OtherActor);
	}
}

void APMTriggerableActor::OnTriggerComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CanBeTriggerBy(OtherActor))
	{
		OnOutTrigger(OtherActor);
	}
}

void APMTriggerableActor::OnTrigger(AActor* OtherTrigger)
{
	bIsTrigger = true;
	ReceiveOnTrigger(OtherTrigger);
}

void APMTriggerableActor::OnOutTrigger(AActor* OtherTrigger)
{
	bIsTrigger = false;
	ReceiveOnOutTrigger(OtherTrigger);
}

