// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable/PMDamageActor.h"
#include "Triggerable/Floor/PMDamageActorOvertimeData.h"
#include "PMDamageActorOvertime.generated.h"

/**
 * This one is an extend one to create trap such as Toxic smoke/river giving damage overtime
 * 
 * TODO:
 * Rename Same as PM Damage floor
 */
UCLASS()
class PLATFORMERMAKER_API APMDamageActorOvertime : public APMDamageActor
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
protected:

	/*
	* Damage rating that floor will gave damage
	*/
	UPROPERTY(EditAnywhere, Category = "PMDamageActor|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "RateDamage"))
	float m_damageRate = .25f;

	/*
	* if set to true, the floor will gave first damage while on trigger
	* otherwise, the first damage will be gave after the first rate
	*/
	UPROPERTY(EditAnywhere, Category = "PMDamageActor|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "IsGivingInstantDamage"))
	bool bIsGivingInstantDamage = false;

	UPROPERTY(Visibleanywhere, BlueprintReadOnly, Category = "PMDamageActor|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "PoolToDamage"))
	TArray<FPMDamageActorOvertimeData> m_poolToDamage;

	/**************************** FUNCTION ******************************/
protected:
	/*
	* Will add an actor to the pool that will take damage
	*/
	UFUNCTION(Blueprintcallable, Category = "PMDamageActor")
	virtual void AddActorOnPool(AActor* Actor);

	/*
	* Will remove actor from pool
	*/
	UFUNCTION(Blueprintcallable, Category = "PMDamageActor")
	virtual void RemoveActorOnPool(AActor* Actor);

	/*
	* Add time on pool object
	* damage if needed
	*/
	UFUNCTION(BlueprintCallable, Category = "PMDamageActor")
	virtual void ApplyDamageTimeToPool(float DeltaTime);

public:
	APMDamageActorOvertime(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void OnTrigger(AActor* OtherTrigger) override;
	virtual void OnOutTrigger(AActor* OtherTrigger) override;

public:

	virtual void Tick(float DeltaTime) override;
};
