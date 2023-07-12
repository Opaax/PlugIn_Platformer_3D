// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable/PMTriggerableActor.h"
#include "PMDamageFloor.generated.h"

/**
 * This class can be use to create basics trap such as spike trap
 * 
 * TODO:
 * Rename this class, it could be use as "ceilings" like spike start from ceiling
 */
UCLASS()
class PLATFORMERMAKER_API APMDamageFloor : public APMTriggerableActor
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:

	/*
	* Base damage that floor will gave to actor that trigger the floor
	*/
	UPROPERTY(EditAnywhere, Category = "Damage", meta = (AllowPrivateAcess = "true", DisplayName = "BaseDamage"))
	float m_baseDamage = 2.f;

	/**************************** FUNCTION ******************************/
protected:
	UFUNCTION(BlueprintCallable)
	virtual void DamageActor(AActor* DamageActor);

	virtual void OnActorDamage(AActor* DamageActor);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnActorDamage"))
	void ReceiveOnActorDamage(AActor* DamageActor);

public:
	APMDamageFloor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void TriggerBeginPlay();

	virtual void OnTrigger(AActor* OtherActor) override;

public:
	virtual void Tick(float DeltaTime) override;
};
