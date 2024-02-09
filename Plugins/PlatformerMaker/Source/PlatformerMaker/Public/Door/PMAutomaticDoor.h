// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Door/PMDoor.h"
#include "PMAutomaticDoor.generated.h"

class UBoxComponent;

/**
 * This Door could be use for many things
 * You can inherit it to make it an automatic door to load levels
 * To change environment
 * Make a break in gameplay, which is good for player
 * It can have many purpose, its up to you.
 */
UCLASS()
class PLATFORMERMAKER_API APMAutomaticDoor : public APMDoor
{
	GENERATED_BODY()

		/**************************** MEMBERS ******************************/
protected:

	/*
	* The component that valid actors will cross to open/close
	*/
	UPROPERTY(VisibleAnywhere, Category = "Door|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> m_triggerComponent;

	/*
	* Classes that can trigger this actor
	* It could be other actor such as character, boxes actor,...
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Setting", meta = (DisplayName = "ValidClassToTrigger"))
	TArray<TSubclassOf<AActor>> m_validClassToTrigger;

	/**************************** FUNCTION ******************************/
protected:
	/*
	* Bind to trigger component overlap event
	*/
	UFUNCTION(BlueprintCallable, Category = "PMAutomaticDoor")
	virtual void BindTriggerComponentEvent();

	/*
	* Begin overlap callback
	*/
	UFUNCTION(BlueprintCallable, Category = "PMAutomaticDoorCallBack")
	virtual void OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*
	* end overlap callback
	*/
	UFUNCTION(BlueprintCallable, Category = "PMAutomaticDoorCallBack")
	virtual void OnBoxComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	APMAutomaticDoor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* Check if the overlap actor is valid
	*/
	UFUNCTION(BlueprintCallable, Category = "PMAutomaticDoor")
	bool CanBeTriggerBy(AActor* OtherActor);

	/**************************** OVERRIDE ******************************/
protected:
	virtual void DoorBeginPlay() override;
};