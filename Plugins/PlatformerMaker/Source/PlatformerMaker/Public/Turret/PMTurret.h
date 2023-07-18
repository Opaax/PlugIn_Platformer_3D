// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PMTurret.generated.h"

//PlugIn
class UPMAIPerceptionComponent;

//Unreal
class UAIPerceptionComponent;
class USceneComponent;

//Plugin
class APMBullet;

/*
* Simple turret that has on sight event.
* If there is any actor on sight, the turret will shoot and try to rotate towards the target
*/
UCLASS()
class PLATFORMERMAKER_API APMTurret : public APawn
{
	GENERATED_BODY()

		/**************************** MEMBERS ******************************/
protected:
	
#pragma region Components

	/*
	* The root Component
	*/
	UPROPERTY(VisibleAnywhere, Category = "Turret|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	/*
	* The perception component
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPMAIPerceptionComponent> m_perceptionComponent;

	/*
	* Where the bullet will spawn
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_spawnBulletPoint;

#pragma endregion Scene component And Actor component

#pragma region  Settings Variables
	/*
	* The shoot rate
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "FireRate"))
	float m_fireRate = .4f;

	/*
	* The type of bullet spawn
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "BulletClass"))
	TSubclassOf<APMBullet> m_bulletClass;
#pragma endregion All Settings variables

#pragma region Runtime Variables
	/*
	* the target that the turret look for
	*/
	UPROPERTY(Visibleanywhere, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "LookAtActor"))
	TObjectPtr<AActor> m_lookAtActor;

	/*
	* The Timer that handle the look at function while there is any actor to look at
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "LookAtTimerHandle"))
	FTimerHandle m_lookAtTimerHandle;

	/*
	* The Timer that handle the shoot while the turret need to shoot
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "ShootTimerHandle"))
	FTimerHandle m_shootTimerHandle;

	/*
	* The current look at rotation value
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "LookAtTimerHandle"))
	FRotator m_lookAtRotation;

	/*
	* all actors that is on sight of the turret
	*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "ActorsOnSight"))
	TArray<AActor*> m_actorsOnSight;
#pragma endregion All Runtime variables

		/**************************** FUNCTION ******************************/
protected:

#pragma region Perception Functions
	/*
	* Listen Perception Component event
	*/
	UFUNCTION(BlueprintCallable)
	virtual void ListenPerceptionComponentEvent();

	/*
	* Call back from perception event
	*/
	UFUNCTION()
	void OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
#pragma endregion All functions for perception

#pragma region Look At Functions
	/*
	* launch the timer to handle look at
	*/
	UFUNCTION(BlueprintCallable)
	virtual void StartLookAtTarget(AActor* Target);

	/*
	* Stop the timer that handle the look at
	*/
	UFUNCTION(BlueprintCallable)
	virtual void StopLookAtTarget();

	/*
	* Compute the look at rotation value
	*/
	UFUNCTION(BlueprintCallable)
	virtual void LookAtTarget();
#pragma endregion All funstion relatif to lookAt

#pragma region Blueprint Event
	/*
	* Callback event for BP when there is a new actor on sight
	*/
	UFUNCTION(BlueprintCallable)
	virtual void OnSight(AActor* Actor);

	/*
	* The blueprint view of the event
	*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSight"))
	void ReceiveOnSight(AActor* Actor);

	/*
	* Callback event for BP when an actor is out sight
	*/
	UFUNCTION(BlueprintCallable)
	virtual void OnOutSight(AActor* Actor);

	/*
	* Callback event for BP every time the rotation value is computed
	*/
	UFUNCTION(BlueprintCallable)
	virtual void OnLookAtRotationComputed(FRotator Rotator);

	/*
	* Callback event for bp when the turret shoot
	*/
	UFUNCTION(BlueprintCallable)
	virtual void OnShoot();

	/*
	* The blueprint view of the event
	*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnOutSight"))
	void ReceiveOnOutSight(AActor* Actor);

	/*
	* The blueprint view of the event
	*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnLookAtRotationComputed"))
	void ReceiveOnLookAtRotationComputed(FRotator Rotator);

	/*
	* The blueprint view of the event
	*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnShoot"))
	void ReceiveOnShoot();
#pragma endregion All implementable event

#pragma region Shoot Functions
	/*
	* launch the timer to handle the shoot
	*/
	UFUNCTION(BlueprintCallable)
	virtual void StartShoot();

	/*
	* Stop the timer that handle the Shoot
	*/
	UFUNCTION(BlueprintCallable)
	virtual void StopShoot();

	/*
	* Spawn new bullet
	* Call the bp event
	*/
	UFUNCTION(BlueprintCallable)
	virtual void Shoot();

	/*
	* Spawn the bullet if a valid bullet class has been set
	*/
	UFUNCTION(BlueprintCallable)
	virtual void SpawnBullet();
#pragma endregion All function relatif to shoot

#pragma region BP Overridable
	/*
	* Overridable function for blueprint
	* Check if the new on sight target should have priority on the look at
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckPriorityLookAtTarget(AActor* NewActor);
	virtual void CheckPriorityLookAtTarget_Implementation(AActor* NewActor);

	/*
	* Overridable function for blueprint
	* Check if there is another target to look at when the current priority target is out of sight
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckForNewPriorityLookAtTarget();
	virtual void CheckForNewPriorityLookAtTarget_Implementation();

	/*
	* Overridable on BP
	* Add a new actor that enter on sight on the sight list
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddActorOnSightList(AActor* NewActor);
	virtual void AddActorOnSightList_Implementation(AActor* NewActor);

	/*
	* Overridable on BP
	* remove a actor that exit from sight on the sight list
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveActorOnSightList(AActor* RemoveActor);
	virtual void RemoveActorOnSightList_Implementation(AActor* RemoveActor);
#pragma endregion All function overridable in blueprint

	UFUNCTION()
	void DrawSightView();
public:
	APMTurret(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

		/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
};
