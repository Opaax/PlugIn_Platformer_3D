// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Triggarable/PMTriggerableActor.h"

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

	if (m_triggerComponent)
		m_triggerComponent->SetupAttachment(m_root);
}

void ATriggerableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATriggerableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

