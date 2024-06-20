// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Platform/PMPlatform.h"
#include "PMTimePlatform.generated.h"

class UBoxComponent;

/**
 * Base class for time platform
 */
UCLASS()
class PLATFORMERMAKER_API APMTimePlatform : public APMPlatform
{
	GENERATED_UCLASS_BODY()
	
protected:
	/*
	* Classes that can trigger this actor
	* It could be other actor such as character, boxes actor,...
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "ValidClassToTrigger"))
	TArray<TSubclassOf<AActor>> m_validClassToTrigger;

	/*
	* How long it take before collisions are removed
	* 
	* Start when trigger
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "TimeToDisable"))
	float m_timeToDisable;

	/*
	* How it take to reset collision
	*
	* Start When collision are disable
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "TimeToReset"))
	float m_timeToReset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", BlueprintGetter = "GetTriggerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> m_triggerComponent;

	FTimerHandle m_timeHandle;

	float m_elapsedTime;

	bool bIsTriggered;

protected:
	UFUNCTION(BlueprintCallable, Category = "TriggerableCallback")
	virtual void OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnTrigger"))
	void ReceiveOnTrigger(AActor* OtherTrigger);

	virtual void OnTrigger(AActor* OtherTrigger);

	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnTimerStart"))
	void ReceiveOnTimerStart();

	virtual void OnTimerStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnCollisionDisable"))
	void ReceiveOnCollisionDisable();

	virtual void OnCollisionDisable();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnCollisionReset"))
	void ReceiveOnCollisionReset();

	virtual void OnCollisionReset();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnTimerUpdated"))
	void ReceiveOnTimerUpdated(float ElapsedTime);

	virtual void OnTimerUpdated(float ElapsedTime);

	UFUNCTION(Category = "TimePlatform")
	void OnTimerUpdate();

	UFUNCTION(Category = "TimePlatform")
	void DisableCollision();

	UFUNCTION(Category = "TimePlatform")
	void StartResetTimer();

	UFUNCTION(BlueprintCallable, Category = "TimePlatform")
	FORCEINLINE float GetTimeRatio() { return m_elapsedTime / m_timeToDisable; }

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TimePlatform|Getter")
	FORCEINLINE UBoxComponent* GetTriggerComponent() const { return m_triggerComponent; }

public:
	virtual void BeginPlay() override;
};
