// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMDoorState.h"
#include "PMDoor.generated.h"

class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPMDoorEventSignature, APMDoor*, Door);

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

#pragma region Delegate
	/*
	* Call when start Open()
	*/
	UPROPERTY(BlueprintAssignable, Category = Event)
	FPMDoorEventSignature OnDoorOpeningDelegate;

	/*
	* Call when start Close()s
	*/
	UPROPERTY(BlueprintAssignable, Category = Event)
	FPMDoorEventSignature OnDoorClosingDelegate;

	/*
	* Call when FinishOpenDoor()
	*/
	UPROPERTY(BlueprintAssignable, Category = Event)
	FPMDoorEventSignature OnDoorOpenDelegate;

	/*
	* Call when FinishCloseDoor()
	*/
	UPROPERTY(BlueprintAssignable, Category = Event)
	FPMDoorEventSignature OnDoorCloseDelegate;
#pragma endregion All Delegates

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

#pragma region Blueprint Events
	/*
	* Call when start to open
	*/
	UFUNCTION(BlueprintCallable, Category = Event)
	virtual void OnOpen();

	/*
	* Blueprint view
	*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnOpen")
	void ReceiveOnOpen();

	/*
	* Should be call when the door is open
	*/
	UFUNCTION(BlueprintCallable, Category = Event)
	virtual void OnFinishOpenDoor();

	/*
	* Blueprint view
	*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnFinishOpenDoor")
	void ReceiveOnFinishOpenDoor();

	/*
	* Call when the door start to close
	*/
	UFUNCTION(BlueprintCallable, Category = Event)
	virtual void OnClose();

	/*
	* Blueprint view
	*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnClose"))
	void ReceiveOnClose();

	/*
	* Should be call when the door is close
	*/
	UFUNCTION(BlueprintCallable, Category = Event)
	virtual void OnFinishCloseDoor();

	/*
	* Blueprint view
	*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnFinishCloseDoor"))
	void ReceiveOnFinishCloseDoor();
#pragma endregion All implementable event

public:
	APMDoor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	virtual void Open();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	virtual void Close();

#pragma region Getter Setter
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE EPMDoorState GetCurrentDoorState() const { return m_currentStateDoor; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentDoorState(EPMDoorState NewState) { m_currentStateDoor = NewState; }
#pragma endregion all getter setter

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
