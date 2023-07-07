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

	/*
	* The root, this is the component returned when you call 'GetRootComponent()'
	*/
	UPROPERTY(Visibleanywhere, Category = "Door|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	/*
	* The current state of the door while playing
	*/
	UPROPERTY(VisibleAnywhere, Category = "Runtime|State", BlueprintGetter = "GetCurrentDoorState", BlueprintSetter = "SetCurrentDoorState", meta = (DisplayName = "CurrentStateDoor"))
	EPMDoorState m_currentStateDoor = EPMDoorState::EDS_Close;

	/*
	* The state that will determine the state at start of the game
	*/
	UPROPERTY(EditAnywhere, Category = "Door|Setting", meta = (DisplayName = "StartStateDoor"))
	EPMDoorStartState m_startStateDoor = EPMDoorStartState::EDSS_Close;

public:
	/**************************** Delegate ******************************/

	/*
	* Call when start Open()
	*/
	UPROPERTY(BlueprintAssignable, Category = Event)
	FDoorEventSignature OnDoorOpeningDelegate;

	/*
	* Call when start Close()s
	*/
	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = Event)
	FDoorEventSignature OnDoorClosingDelegate;

	/*
	* Call when FinishOpenDoor()
	*/
	UPROPERTY(BlueprintAssignable, Category = Event)
	FDoorEventSignature OnDoorOpenDelegate;

	/*
	* Call when FinishCloseDoor()
	*/
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

	/*
	* Will determine at beginplay if the door is open or not
	*/
	UFUNCTION(BlueprintCallable)
	virtual void CheckForStartState();

	/**************************** Open/Close ******************************/

	/*
	*/
	virtual void OnOpen();

	UFUNCTION(BlueprintCallable)
	virtual void OnFinishOpenDoor();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnOpen")
	void ReceiveOnOpen();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnFinishOpenDoor")
	void ReceiveOnFinishOpenDoor();

	virtual void OnClose();

	UFUNCTION(BlueprintCallable)
	virtual void OnFinishCloseDoor();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnClose"))
	void ReceiveOnClose();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnFinishCloseDoor"))
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
