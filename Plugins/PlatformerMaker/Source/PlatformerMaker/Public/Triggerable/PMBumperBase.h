//2023 Copyright Enguerran COBERT, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Public/Triggerable/PMTriggerableActor.h"
#include "PMBumperBase.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBumpedEventSignature);

UCLASS()
class PLATFORMERMAKER_API APMBumperBase : public APMTriggerableActor
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:

	/**************************** Settings ******************************/

	/*
	* base force that bumper while apply on character and actors
	* if the overlapped actor is a character, consider that the bumper while launch character, otherwise will impulse on the root
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bumper|Setting", meta = (DisplayName = "BumperCharacterForce"))
	float m_bumperForce = 2000.f;
	
	/*
	* False = keep the last character movement velocity XY
	* TRUE = Use custom XY Velocity
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bumper|Setting", meta = (DisplayName = "bOverrideXYCharacterVelocity"))
	bool bOverrideXYCharacterVelocity = false;

	/*
	* False = keep the last character movement velocity XY
	* TRUE = Use custom XY Velocity
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bumper|Setting", meta = (EditCondition = "bOverrideXYCharacterVelocity", DisplayName = "CustomCharacterXYVelocity"))
	FVector m_customCharacterXYVelocity = FVector();

	/*
	* Basic Actor do not work as same as character
	* The character is manage by it's movement component
	* Actors by it's root component with physics enable
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bumper|Setting", meta = (DisplayName = "BumperActorForce"))
	float m_bumperActorForceFactor = 100.f;

	/**************************** End Settings ******************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UStaticMeshComponent> m_baseBumper;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UStaticMeshComponent> m_topBumper;

public:

	UPROPERTY(BlueprintAssignable, Category = "Bumper|Delegate")
	FBumpedEventSignature OnBumpedDelegate;

	/**************************** FUNCTION ******************************/
public:

	APMBumperBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void OnBumped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events", meta = (DisplayName = "OnBumped"))
	void ReceiveOnBumped();

	virtual void OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	/**************************** OVERRIDE ******************************/
protected:

	virtual void BeginPlay() override;

	virtual void TriggerBeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

};
