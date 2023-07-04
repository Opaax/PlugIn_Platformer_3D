// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Triggerable/PMDamageFloorOvertime.h"

APMDamageFloorOvertime::APMDamageFloorOvertime(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

void APMDamageFloorOvertime::OnTrigger(AActor* OtherTrigger)
{
	ReceiveOnTrigger(OtherTrigger);
}

void APMDamageFloorOvertime::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DoDamage
}

void APMDamageFloorOvertime::AddActorOnPool(AActor* Actor)
{
	FPMDamageFloorOvertimeData lData = FPMDamageFloorOvertimeData();
	lData.Actor = Actor;

	//If there empty
	//we can add directly without checking if the actor is already on pool
	//else check
	if (m_poolToDamage.IsEmpty())
	{
		m_poolToDamage.Add(lData);
	}
	else
	{
		for (int i = 0; i < m_poolToDamage.Num(); i++)
		{
			if (m_poolToDamage[i].Actor == Actor)
			{
				return;
			}
		}

		m_poolToDamage.Add(lData);
	}
}

void APMDamageFloorOvertime::RemoveActorOnPool(AActor* Actor)
{
	for (int i = 0; i < m_poolToDamage.Num(); i++)
	{
		if (m_poolToDamage[i].Actor == Actor)
		{
			m_poolToDamage.RemoveAt(i);
			return;
		}
	}
}
