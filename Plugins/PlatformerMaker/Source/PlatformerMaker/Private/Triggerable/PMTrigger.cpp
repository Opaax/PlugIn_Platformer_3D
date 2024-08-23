// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Triggerable/PMTrigger.h"
#include "Triggerable/PMTriggerable.h"
#include "Utils/DebugMacro.h"

//Unreal
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineTypes.h"

APMTrigger::APMTrigger(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass(TriggerComponentName,UBoxComponent::StaticClass()))
{
	m_childClass = NULL;
}

#if WITH_EDITOR
void APMTrigger::UpdateChildren()
{
	//Clear and remove old actor spawn
	if (!m_spawnChildren.IsEmpty())
	{
		for (int i = m_spawnChildren.Num() - 1; i >= 0; i--)
		{
			m_spawnChildren[i]->Destroy();
		}

		m_spawnChildren.Empty();
	}

	FTransform lCurrentTransform = GetTransform();
	FVector lLocation = GetActorLocation();
	FVector lForward = GetActorForwardVector();
	
	AActor* lCurrentActor = nullptr;
	
	for (int32 i = 0; i < m_numChildToSpawn; i++)
	{
		float lDist = i * m_spacingBetweenChild;
		lLocation = GetActorLocation() + (lForward * lDist);
		lCurrentTransform.SetLocation(lLocation);
		
		FActorSpawnParameters lParams;
		lParams.Owner = this;
		
		lCurrentActor = GetWorld()->SpawnActor<AActor>(m_childClass, lCurrentTransform, lParams);

		FAttachmentTransformRules lAttachRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
		lCurrentActor->AttachToActor(this, lAttachRules);
		
		m_spawnChildren.Add(lCurrentActor);
	}
}

void APMTrigger::SpawnChildrenMissingChildren(const int32 Num)
{
	if(!IsValid(m_childClass))
	{
		return;
	}

	AActor* lCurrentActor = nullptr;

	for (int32 i = 0; i < Num; i++)
	{
		FTransform lCurrentTransform = GetTransform();
		FActorSpawnParameters lParams;
		lParams.Owner = this;
		
		lCurrentActor = GetWorld()->SpawnActor<AActor>(m_childClass, lCurrentTransform,lParams);

		FAttachmentTransformRules lAttachRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);

		lCurrentActor->AttachToActor(this,lAttachRules);

		m_spawnChildren.Add(lCurrentActor);
	}
}

void APMTrigger::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.Property)
	{
		FName lNameProp = PropertyChangedEvent.Property->NamePrivate;

		if(lNameProp == "m_childClass" || lNameProp == "m_numChildToSpawn" || lNameProp == "m_spacingBetweenChild")
		{
			UpdateChildren();
		}
	}
}

void APMTrigger::PostEditUndo()
{
	Super::PostEditUndo();

	UpdateChildren();
}

void APMTrigger::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	UpdateChildren();
}
#endif