// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable/PMTriggerableActor.h"
#include "PMDamageActor.generated.h"

/**
 * This class can be use to create basics trap such as spike trap
 */
UCLASS()
class PLATFORMERMAKER_API APMDamageActor : public APMTriggerableActor
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:

	/*
	* Base damage that floor will gave to actor that trigger the floor
	*/
	UPROPERTY(EditAnywhere, Category = "PMDamageActor|Setting", meta = (AllowPrivateAcess = "true", DisplayName = "BaseDamage"))
	float m_baseDamage = 2.f;

	/**************************** FUNCTION ******************************/
protected:
	UFUNCTION(BlueprintCallable, Category = "PMDamageActor")
	virtual void DamageActor(AActor* DamageActor);

	virtual void OnActorDamage(AActor* DamageActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PMDamageActor", meta = (DisplayName = "OnActorDamage"))
	void ReceiveOnActorDamage(AActor* DamageActor);

public:
	APMDamageActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void TriggerBeginPlay();

	virtual void OnTrigger(AActor* OtherActor) override;

public:
	virtual void Tick(float DeltaTime) override;
};
