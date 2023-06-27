// Copyright Enguerran COBERT, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMBumperBase.generated.h"

class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBumpedEventSignature);

UCLASS()
class PLATFORMERMAKER_API APMBumperBase : public AActor
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:

	/**************************** Settings ******************************/

	/*
	* base force that bumper while apply on character and actors
	* if the overlapped actor is a character, consider that the bumper while launch character, otherwise will impulse on the root
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bumper|Setting", meta = (DisplayName = "BumperForce"))
	float m_bumperForce = 2000.f;

	/*
	* Basic Actor do not work as same as character
	* The character is manage by it's movement component
	* Actors by it's root component with physics enable
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bumper|Setting", meta = (DisplayName = "BumperForce"))
	float m_bumperActorForceFactor = 100.f;

	/*
	* Classes that can be Bumped
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bumper|Setting", meta = (DisplayName = "ValidClassToBump"))
	TArray<TSubclassOf<AActor>> m_validClassToBump;

	/**************************** End Settings ******************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> m_boxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UStaticMeshComponent> m_baseBumper;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UStaticMeshComponent> m_topBumper;

public:

	UPROPERTY(BlueprintAssignable, Category = Bumped)
	FBumpedEventSignature OnBumpedDelegate;

	/**************************** FUNCTION ******************************/
public:

	APMBumperBase();

protected:

	virtual void OnBumped();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnBumped"))
	void ReceiveOnBumped();

	UFUNCTION(BlueprintCallable)
	virtual void BindBoxComponentEvent();

	UFUNCTION(BlueprintCallable)
	virtual void OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**************************** OVERRIDE ******************************/
protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

};
