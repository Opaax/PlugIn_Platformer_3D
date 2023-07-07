// Copyright Enguerran COBERT, Inc. All Rights Reserved.

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
	UFUNCTION(BlueprintCallable)
	virtual void BindTriggerComponentEvent();

	UFUNCTION(BlueprintCallable)
	virtual void OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	virtual void OnBoxComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	APMAutomaticDoor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	bool CanBeTriggerBy(AActor* OtherActor);

	/**************************** OVERRIDE ******************************/
protected:
	virtual void DoorBeginPlay() override;
};