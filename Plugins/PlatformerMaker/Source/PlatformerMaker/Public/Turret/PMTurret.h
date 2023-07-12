// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PMTurret.generated.h"

class UAIPerceptionComponent;
class USceneComponent;

class APMBullet;

/*
*/
UCLASS()
class PLATFORMERMAKER_API APMTurret : public APawn
{
	GENERATED_BODY()

		/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> m_perceptionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Turret|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_spawnBulletPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "FireRate"))
	float m_fireRate = .4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "BulletClass"))
	TSubclassOf<APMBullet> m_bulletClass;

	UPROPERTY(Visibleanywhere, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "LookAtActor"))
	TObjectPtr<AActor> m_lookAtActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "LookAtTimerHandle"))
	FTimerHandle m_lookAtTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "ShootTimerHandle"))
	FTimerHandle m_shootTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "LookAtTimerHandle"))
	FRotator m_lookAtRotation;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "ActorsOnSight"))
	TArray<AActor*> m_actorsOnSight;

		/**************************** FUNCTION ******************************/
protected:
	UFUNCTION(BlueprintCallable)
	virtual void ListenPerceptionComponentEvent();

	UFUNCTION()
	void OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckPriorityLookAtTarget(AActor* NewActor);
	virtual void CheckPriorityLookAtTarget_Implementation(AActor* NewActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckForNewPriorityLookAtTarget();
	virtual void CheckForNewPriorityLookAtTarget_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void StartLookAtTarget(AActor* Target);

	UFUNCTION(BlueprintCallable)
	virtual void StartShoot();

	UFUNCTION(BlueprintCallable)
	virtual void StopShoot();

	UFUNCTION(BlueprintCallable)
	virtual void StopLookAtTarget();

	UFUNCTION(BlueprintCallable)
	virtual void LookAtTarget();

	UFUNCTION(BlueprintCallable)
	virtual void Shoot();

	UFUNCTION(BlueprintCallable)
	virtual void OnSight(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	virtual void OnOutSight(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	virtual void OnLookAtRotationComputed(FRotator Rotator);

	UFUNCTION(BlueprintCallable)
	virtual void OnShoot();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSight"))
	void ReceiveOnSight(AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnOutSight"))
	void ReceiveOnOutSight(AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnLookAtRotationComputed"))
	void ReceiveOnLookAtRotationComputed(FRotator Rotator);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnShoot"))
	void ReceiveOnShoot();

	UFUNCTION(BlueprintCallable)
	virtual void SpawnBullet();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddActorOnSightList(AActor* NewActor);
	virtual void AddActorOnSightList_Implementation(AActor* NewActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveActorOnSightList(AActor* RemoveActor);
	virtual void RemoveActorOnSightList_Implementation(AActor* RemoveActor);

public:
	APMTurret(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

		/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
