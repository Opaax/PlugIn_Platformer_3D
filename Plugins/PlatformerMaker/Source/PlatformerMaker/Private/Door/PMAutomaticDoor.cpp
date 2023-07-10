// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Door/PMAutomaticDoor.h"
#include "PlatformerMaker.h"

//Unreal
#include "Components/BoxComponent.h"

APMAutomaticDoor::APMAutomaticDoor(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	m_triggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	m_triggerComponent->SetupAttachment(m_root);
}

void APMAutomaticDoor::DoorBeginPlay()
{
	Super::DoorBeginPlay();

	BindTriggerComponentEvent();
}

void APMAutomaticDoor::BindTriggerComponentEvent()
{
	if (m_triggerComponent)
	{
		m_triggerComponent->OnComponentBeginOverlap.AddDynamic(this, &APMAutomaticDoor::OnBoxComponentOverlapped);
		m_triggerComponent->OnComponentEndOverlap.AddDynamic(this, &APMAutomaticDoor::OnBoxComponentEndOverlapped);
	}
	else
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, trigger component is not constructed"), *GetName());
	}
}

void APMAutomaticDoor::OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanBeTriggerBy(OtherActor))
	{
		switch (m_currentStateDoor)
		{
		case EPMDoorState::EDS_Open:
			Close();
			break;
		case EPMDoorState::EDS_Close:
		default:
			Open();
			break;
		}	
	}
}

void APMAutomaticDoor::OnBoxComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CanBeTriggerBy(OtherActor))
	{
		switch (m_currentStateDoor)
		{
		case EPMDoorState::EDS_Open:
			Close();
			break;
		case EPMDoorState::EDS_Close:
		default:
			Open();
			break;
		}
	}
}

bool APMAutomaticDoor::CanBeTriggerBy(AActor* OtherActor)
{
	return IsValid(OtherActor) && m_validClassToTrigger.Contains(OtherActor->GetClass());
}
