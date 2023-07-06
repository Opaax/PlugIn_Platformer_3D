// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMDoorState.h"
#include "PMDoor.generated.h"

class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDoorEventSignature, APMDoor*, Door);

/*
* Simple class to manage event for doors
*/
UCLASS()
class PLATFORMERMAKER_API APMDoor : public AActor
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(Visibleanywhere, Category = "Door|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	UPROPERTY(VisibleAnywhere, Category = "Runtime|State", BlueprintGetter = "GetCurrentDoorState", BlueprintSetter = "SetCurrentDoorState", meta = (DisplayName = "CurrentStateDoor"))
	EPMDoorState m_currentStateDoor = EPMDoorState::EDS_Close;

	UPROPERTY(EditAnywhere, Category = "Door|Setting", meta = (DisplayName = "StartStateDoor"))
	EPMDoorStartState m_startStateDoor = EPMDoorStartState::EDSS_Close;

	/**************************** Delegate ******************************/
	UPROPERTY(BlueprintAssignable, Category = Event)
	FDoorEventSignature OnDoorOpeningDelegate;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = Event)
	FDoorEventSignature OnDoorClosingDelegate;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FDoorEventSignature OnDoorOpenDelegate;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FDoorEventSignature OnDoorCloseDelegate;
	/**************************** End Delegate ******************************/

	/**************************** FUNCTION ******************************/
protected:
	/*
	* Call after regular beginplay
	*/
	UFUNCTION(BlueprintCallable)
	virtual void DoorBeginPlay();

	UFUNCTION(BlueprintCallable)
	virtual void CheckForStartState();

	/**************************** Open/Close ******************************/
	virtual void OnOpen();

	virtual void OnFinishOpenDoor();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnOpen")
	void ReceiveOnOpen();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, DisplayName = "OnFinishOpenDoor")
	void ReceiveOnFinishOpenDoor();

	virtual void OnClose();

	virtual void OnFinishCloseDoor();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnClose"))
	void ReceiveOnClose();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "OnFinishCloseDoor"))
	void ReceiveOnFinishCloseDoor();
	/**************************** End Open/Close ******************************/

public:
	APMDoor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	virtual void Open();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	virtual void Close();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE EPMDoorState GetCurrentDoorState() const { return m_currentStateDoor; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentDoorState(EPMDoorState NewState) { m_currentStateDoor = NewState; }

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
