// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable/PMDamageFloor.h"
#include "Triggerable/Floor/PMDamageFloorOvertimeData.h"
#include "PMDamageFloorOvertime.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API APMDamageFloorOvertime : public APMDamageFloor
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
protected:

	/*
	* Damage rating that floor will gave damage
	*/
	UPROPERTY(EditAnywhere, Category = "Damage", meta = (AllowPrivateAccess = "true", DisplayName = "RateDamage"))
	float m_damageRate = .25f;

	/*
	* if set to true, the floor will gave first damage while on trigger
	* otherwise, the first damage will be gave after the first rate
	*/
	UPROPERTY(EditAnywhere, Category = "Setting", meta = (AllowPrivateAccess = "true", DisplayName = "RateDamage"))
	bool bIsGivingInstantDamage = false;

	UPROPERTY(Visibleanywhere, BlueprintReadOnly, Category = "Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "PoolToDamage"))
	TArray<FPMDamageFloorOvertimeData> m_poolToDamage;

	/**************************** FUNCTION ******************************/
protected:
	/*
	* Will add an actor to the pool that will take damage
	*/
	UFUNCTION(Blueprintcallable)
	virtual void AddActorOnPool(AActor* Actor);

	/*
	* Will remove actor from pool
	*/
	UFUNCTION(Blueprintcallable)
	virtual void RemoveActorOnPool(AActor* Actor);

	/*
	* Add time on pool object
	* damage if needed
	*/
	UFUNCTION(BlueprintCallable)
	virtual void ApplyDamageTimeToPool(float DeltaTime);

public:
	APMDamageFloorOvertime(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void OnTrigger(AActor* OtherTrigger) override;

public:

	virtual void Tick(float DeltaTime) override;
};
