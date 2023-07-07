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

	UFUNCTION(BlueprintCallable)
	virtual void StartInteraction();

	UFUNCTION(BlueprintCallable)
	virtual void EnableInteraction();

	UFUNCTION(BlueprintCallable)
	virtual void DisableInteraction();

	UFUNCTION(BlueprintCallable)
	virtual void StopInteraction();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveStartInteraction();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveStopInteraction();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveEnableInteraction();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveDisableInteraction();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool GetIsInteractable() const { return bInteractable; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool GetIsActivated() const { return bActivated; }

	UFUNCTION(BlueprintCallable)
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
