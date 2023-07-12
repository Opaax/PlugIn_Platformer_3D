// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMBullet.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class PLATFORMERMAKER_API APMBullet : public AActor
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
protected:
	/*
	* The root Component, where all scene components will be attached
	*/
	UPROPERTY(VisibleAnywhere, Category = "Bullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	/*
	* Trigger component.
	*/
	UPROPERTY(VisibleAnywhere, Category = "Bullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> m_trigger;

	/*
	* The mesh of the bullet
	*/
	UPROPERTY(VisibleAnywhere, Category = "Bullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> m_bulletMesh;

	/*
	* Movement behavior of the bullets
	*/
	UPROPERTY(VisibleAnywhere, Category = "Bullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> m_projectileMoveComp;

	/**************************** FUNCTION ******************************/
protected:

	/*
	* Custom Beginplay called after Unreal Beginplay
	*/
	UFUNCTION(BlueprintCallable)
	virtual void BulletBeginPlay();

	/*
	*Bind trigger component events
	*/
	UFUNCTION(BlueprintCallable)
	virtual void BindTriggerComponentEvent();

	/*
	* Called back for OnBeginOverlap from Trigger Component
	*/
	UFUNCTION(BlueprintCallable)
	virtual void OnComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*
	* Event For blueprint
	*/
	UFUNCTION(BlueprintCallable)
	virtual void OnHit(AActor* HitActor, UPrimitiveComponent* HitComponent, const FHitResult& SweepResult);

	/*
	* Blueprint view from OnHit
	*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnHit"))
	void ReceiveOnHit(AActor* HitActor, UPrimitiveComponent* HitComponent, const FHitResult& SweepResult);

public:
	APMBullet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
