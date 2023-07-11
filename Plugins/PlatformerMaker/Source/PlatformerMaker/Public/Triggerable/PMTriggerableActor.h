// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMTriggerableActor.generated.h"

class UShapeComponent;
class USceneComponent;

/*
* If you want to inherit this class, by default the trigger is a 'Shape Component'
* You can make it what ever shape you want by using 'TriggerComponentName' in the Constructor
* Like: APMBumperBase::APMBumperBase(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<UBoxComponent>(TriggerComponentName))
* 
* All inherited classe need to set it's trigger shape type
*/
UCLASS()
class PLATFORMERMAKER_API APMTriggerableActor : public AActor
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:

	/*
	* Classes that can trigger this actor
	* It could be other actor such as character, boxes actor,...
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triggerable|Setting", meta = (DisplayName = "ValidClassToTrigger"))
	TArray<TSubclassOf<AActor>> m_validClassToTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", BlueprintGetter = "GetTriggerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShapeComponent> m_triggerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

public:

	static FName TriggerComponentName;

	/**************************** FUNCTION ******************************/
protected:

	UFUNCTION(BlueprintCallable)
	virtual void BindTriggerComponentEvent();

	UFUNCTION(BlueprintCallable)
	virtual void RemoveBindTriggerComponentEvent();

	/*
	* Custom begin play for trigger classes
	*/
	UFUNCTION(BlueprintCallable)
	virtual void TriggerBeginPlay();

	UFUNCTION(BlueprintCallable)
	virtual void OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintCallable)
	virtual void OnBoxComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnTrigger(AActor* OtherTrigger);
	virtual void OnOutTrigger(AActor* OtherTrigger);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTrigger"))
	void ReceiveOnTrigger(AActor* OtherTrigger);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnOutTrigger"))
	void ReceiveOnOutTrigger(AActor* OtherTrigger);

public:	
	APMTriggerableActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	bool CanBeTriggerBy(AActor* OtherActor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE UShapeComponent* GetTriggerComponent() const { return m_triggerComponent; }

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
