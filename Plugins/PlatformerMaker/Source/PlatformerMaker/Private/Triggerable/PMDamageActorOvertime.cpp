// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Triggerable/PMDamageActorOvertime.h"

APMDamageActorOvertime::APMDamageActorOvertime(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	//We need the tick in this actor
	PrimaryActorTick.bCanEverTick = true;
}

void APMDamageActorOvertime::OnTrigger(AActor* OtherTrigger)
{
	ReceiveOnTrigger(OtherTrigger);

	AddActorOnPool(OtherTrigger);
}

void APMDamageActorOvertime::OnOutTrigger(AActor* OtherTrigger)
{
	ReceiveOnOutTrigger(OtherTrigger);

	RemoveActorOnPool(OtherTrigger);
}

void APMDamageActorOvertime::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Add time to pool, I don't want add damage to actor that just enter in. it does not feel right
	// so I did a pool where I actor got its own time in this floor
	ApplyDamageTimeToPool(DeltaTime);
}

void APMDamageActorOvertime::ApplyDamageTimeToPool(float DeltaTime)
{
	//Check if the pool is not empty
	if (!m_poolToDamage.IsEmpty())
	{
		//iterate the pool
		for (int32 i = 0; i < m_poolToDamage.Num(); i++)
		{
			//check the actor inside the struct, we make sure the struct is valid by that
			if (IsValid(m_poolToDamage[i].Actor))
			{
				//add time
				m_poolToDamage[i].CurrentTime += DeltaTime;

				//check the time by the rate of this floor
				if (m_poolToDamage[i].CurrentTime >= m_damageRate)
				{
					//add damage if damageable
					if (m_poolToDamage[i].Actor->CanBeDamaged())
					{
						DamageActor(m_poolToDamage[i].Actor);

						//Reset the time
						m_poolToDamage[i].Reset();
					}
				}
			}
		}
	}
}

void APMDamageActorOvertime::AddActorOnPool(AActor* Actor)
{
	FPMDamageActorOvertimeData lData = FPMDamageActorOvertimeData();
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

	//Add damage if this floor do instant damage
	if (bIsGivingInstantDamage)
	{
		DamageActor(Actor);
	}
}

void APMDamageActorOvertime::RemoveActorOnPool(AActor* Actor)
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
