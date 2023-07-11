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

	UPROPERTY(VisibleAnywhere, Category = "Bullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	UPROPERTY(VisibleAnywhere, Category = "Bullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> m_trigger;

	UPROPERTY(VisibleAnywhere, Category = "Bullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> m_bulletMesh;

	UPROPERTY(VisibleAnywhere, Category = "Bullet|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> m_projectileMoveComp;

	/**************************** FUNCTION ******************************/
protected:
	UFUNCTION(BlueprintCallable)
	virtual void BulletBeginPlay();

	UFUNCTION(BlueprintCallable)
	virtual void OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	APMBullet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
