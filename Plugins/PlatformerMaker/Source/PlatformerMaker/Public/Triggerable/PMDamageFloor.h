// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable/PMTriggerableActor.h"
#include "PMDamageFloor.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API APMDamageFloor : public ATriggerableActor
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:

	UPROPERTY(EditAnywhere, Category = "Damage", meta = (AllowPrivateAcess = "true", DisplayName = "BaseDamage"))
	float m_baseDamage = 2.f;

	/**************************** FUNCTION ******************************/
protected:
public:
	APMDamageFloor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void TriggerBeginPlay();

	virtual void OnTrigger(AActor* OtherActor) override;

public:
	virtual void Tick(float DeltaTime) override;
};
