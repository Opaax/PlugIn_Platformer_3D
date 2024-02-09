//2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
#include "../Triggerable/PMDamageActor.h"
#include "PMBullet.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class PLATFORMERMAKER_API APMBullet : public APMDamageActor
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
protected:

	/*
	* The mesh of the bullet
	*/
	UPROPERTY(VisibleAnywhere, Category = "PMBullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> m_bulletMesh;

	/*
	* Movement behavior of the bullets
	*/
	UPROPERTY(VisibleAnywhere, Category = "PMBullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> m_projectileMoveComp;

	/**************************** FUNCTION ******************************/
protected:
	/*
	* Event For blueprint
	*/
	UFUNCTION(BlueprintCallable, Category = "Event")
	virtual void OnHit(AActor* HitActor, UPrimitiveComponent* HitComponent, const FHitResult& SweepResult);

	/*
	* Blueprint view from OnHit
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnHit"))
	void ReceiveOnHit(AActor* HitActor, UPrimitiveComponent* HitComponent, const FHitResult& SweepResult);

public:
	APMBullet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

	virtual void OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void BeginDestroy() override;
};
