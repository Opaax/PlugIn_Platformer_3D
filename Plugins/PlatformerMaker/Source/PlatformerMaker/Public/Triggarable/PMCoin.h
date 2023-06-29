// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Public/Triggarable/PMTriggerableActor.h"
#include "PMCoin.generated.h"

class UStaticMeshComponent;
class URotatingMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCoinEventSignature, int32, CoinValue, AActor*, TriggerBy);

/*
* Default Coin class.
* By default there is a rotating component, this component is optional and can be remove by the child class.
* DoNotCreateDefaultSubobject with the subobject's name (RotatingComponentName).
*/
UCLASS()
class PLATFORMERMAKER_API APMCoin : public ATriggerableActor
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "CoinValue"))
	int32 m_coinValue = 1;

	UPROPERTY(Visibleanywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> m_coinMesh;

	UPROPERTY(Visibleanywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URotatingMovementComponent> m_rotatingComponent;

public:

	static FName RotatingComponentName;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FCoinEventSignature OnCoinTriggerDelegate;

	/**************************** FUNCTION ******************************/
protected:
	virtual void OnCoinTrigger(int32 Coinvalue, AActor* TriggerBy);

	/*
	* Return the actor that trigger the coin in case some actor do other effect than get coin value
	* Could be negative things
	*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnCoinTrigger"))
	void ReceiveOnCoinTrigger(int32 Coinvalue, AActor* TriggerBy);

public:
	APMCoin(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

	virtual void OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:	
	virtual void Tick(float DeltaTime) override;

};
