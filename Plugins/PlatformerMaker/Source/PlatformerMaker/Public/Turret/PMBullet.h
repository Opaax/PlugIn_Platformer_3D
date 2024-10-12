//2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable/PMTriggerableActor.h"
#include "PMBullet.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class PLATFORMERMAKER_API APMBullet : public APMTriggerableActor
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

	/*
	* Base damage that floor will give to actor that trigger the floor
	*/
	UPROPERTY(EditAnywhere, Category = "PMBullet|Setting", meta = (AllowPrivateAcess = "true", DisplayName = "BaseDamage"))
	float m_baseDamage = 2.f;

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

	virtual void OnActorDamage(AActor* DamageActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PMDamageActor", meta = (DisplayName = "OnActorDamage"))
	void ReceiveOnActorDamage(AActor* DamageActor);

	UFUNCTION(BlueprintCallable, Category = "PMDamageActor")
	virtual void DamageActor(AActor* DamageActor);
	
public:
	APMBullet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;
	virtual void OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnTrigger(AActor* OtherActor) override;
	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void BeginDestroy() override;
};
