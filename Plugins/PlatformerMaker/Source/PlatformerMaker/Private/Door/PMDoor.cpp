// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Door/PMDoor.h"
#include "PlatformerMaker.h"

//Unreal
#include "Components/SceneComponent.h"

APMDoor::APMDoor(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(m_root);
}

void APMDoor::BeginPlay()
{
	Super::BeginPlay();

	//Custom begin play
	DoorBeginPlay();
}

void APMDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APMDoor::DoorBeginPlay()
{
	CheckForStartState();
}

void APMDoor::CheckForStartState()
{
	//Switch on start state to determine if the door is open or close
	switch (m_startStateDoor)
	{
	case EPMDoorStartState::EDSS_Open:
		Open();
		m_currentStateDoor = EPMDoorState::EDS_Open;
		break;
	case EPMDoorStartState::EDSS_Close:
	default:
		Close();
		m_currentStateDoor = EPMDoorState::EDS_Close;
		break;
	}
}

void APMDoor::OnOpen()
{
	ReceiveOnOpen();
}

void APMDoor::OnFinishOpenDoor()
{
	m_currentStateDoor = EPMDoorState::EDS_Open;

	OnDoorOpenDelegate.Broadcast(this);

	ReceiveOnFinishOpenDoor();

	UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, The door finish as open"), *GetName());
}

void APMDoor::OnClose()
{
	ReceiveOnClose();
}

void APMDoor::OnFinishCloseDoor()
{
	//Set state as close
	m_currentStateDoor = EPMDoorState::EDS_Close;

	//call the delegate to notify listeners
	OnDoorCloseDelegate.Broadcast(this);

	//call bp event to notify the blueprint
	ReceiveOnFinishCloseDoor();

	UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, The door finish as close"), *GetName());
}

void APMDoor::Open()
{
	//Set state as open on going
	m_currentStateDoor = EPMDoorState::EDS_OnOpening;

	//call the delegate to notify listeners
	OnDoorOpeningDelegate.Broadcast(this);

	//call bp event to notify blueprint
	OnOpen();

	UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, Door Open"), *GetName());
}

void APMDoor::Close()
{
	m_currentStateDoor = EPMDoorState::EDS_OnClosing;

	OnDoorClosingDelegate.Broadcast(this);

	OnClose();

	UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, Door Close"), *GetName());
}

