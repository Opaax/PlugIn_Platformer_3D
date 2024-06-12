// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Platform/PMPlatform.h"
#include "PMOneWayPlatform.generated.h"

class UBoxComponent;
class USceneComponent;

#if WITH_EDITORONLY_DATA
class UArrowComponent;
#endif //WITH_EDITORONLY_DATA

/**
 * One way Platform.
 * 
 * Custom class set, can traverse this platform in certain direction
 */
UCLASS()
class PLATFORMERMAKER_API APMOneWayPlatform : public APMPlatform
{
	GENERATED_UCLASS_BODY()
	
protected:
	/*
	* Classes that can trigger this actor
	* It could be other actor such as character, boxes actor,...
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "ValidClassToTrigger"))
	TArray<TSubclassOf<AActor>> m_validClassToTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", BlueprintGetter = "GetTriggerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> m_triggerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", BlueprintGetter = "GetOneWayDirectionComp", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_oneWayDirection;

	UPROPERTY(VisibleAnywhere, Category = "Runtime")
	TObjectPtr<AActor> m_currentTriggerActor;

#if WITH_EDITORONLY_DATA
	/** Component shown in the editor only to indicate oneway facing facing */
	UPROPERTY()
	TObjectPtr<UArrowComponent> m_arrowComponent;
#endif

protected:
	UFUNCTION(BlueprintCallable, Category = "TriggerableCallback")
	virtual void OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintCallable, Category = "TriggerableCallback")
	virtual void OnTriggerComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnTrigger"))
	void ReceiveOnTrigger(AActor* OtherTrigger);

	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnOutTrigger"))
	void ReceiveOnOutTrigger(AActor* OtherTrigger);

	virtual void OnTrigger(AActor* OtherTrigger);
	virtual void OnOutTrigger(AActor* OtherTrigger);

	UFUNCTION(BlueprintNativeEvent, Category = "OneWay")
	void ComputeOneWayCollision();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OneWay|Getter")
	FORCEINLINE UBoxComponent* GetTriggerComponent() const { return m_triggerComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OneWay|Getter")
	FORCEINLINE USceneComponent* GetOneWayDirectionComp() const { return m_oneWayDirection; }

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
