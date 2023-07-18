// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PMInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPMInteractionToggleDelegateSignature, UPMInteractionComponent*, ComponentToggled, bool, Toggle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPMInteractionDelegateSignature, bool, InteractionStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPMInteractionActivatedDelegateSignature, bool, Activated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLATFORMERMAKER_API UPMInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, BlueprintGetter = "GetIsInteractable", Category = "Interaction|Runtime", meta = (DisplayName = "Interactable"))
	bool bInteractable = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintGetter = "GetIsActivated", BlueprintSetter = "SetActivated", Category = "Interaction|Runtime", meta = (DisplayName = "Activated"))
	bool bActivated = false;

public:

	UPROPERTY(BlueprintAssignable, Category = Event)
	FPMInteractionToggleDelegateSignature OnInteractionToggled;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FPMInteractionDelegateSignature OnInteract;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FPMInteractionActivatedDelegateSignature OnInteractionActivated;
	
	/**************************** FUNCTION ******************************/
public:	
	UPMInteractionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void StartInteraction();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void EnableInteraction();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void DisableInteraction();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void StopInteraction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void ReceiveStartInteraction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void ReceiveStopInteraction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void ReceiveEnableInteraction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void ReceiveDisableInteraction();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Getter")
	FORCEINLINE bool GetIsInteractable() const { return bInteractable; }

	UFUNCTION(BlueprintCallable, BlueprintPure , Category = "Getter")
	FORCEINLINE bool GetIsActivated() const { return bActivated; }

	UFUNCTION(BlueprintCallable, Category = "Setter")
	FORCEINLINE void SetActivated(bool NewActivated)
	{ 
		bActivated = NewActivated;
		OnInteractionActivated.Broadcast(bActivated);
	}

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		
};
