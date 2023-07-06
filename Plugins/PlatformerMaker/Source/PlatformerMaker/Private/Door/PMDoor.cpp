// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Door/PMDoor.h"

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
	switch (m_startStateDoor)
	{
	case EPMDoorStartState::EDSS_Open:
		Open();
		break;
	case EPMDoorStartState::EDSS_Close:
	default:
		Close();
		break;
	}
}

void APMDoor::OnOpen()
{
	ReceiveOnOpen();
}

void APMDoor::OnFinishOpenDoor()
{
	ReceiveOnFinishOpenDoor();

	SetCurrentDoorState(EPMDoorState::EDS_Open);

	OnDoorOpenDelegate.Broadcast(this);
}

void APMDoor::OnClose()
{
	ReceiveOnClose();
}

void APMDoor::OnFinishCloseDoor()
{
	ReceiveOnFinishCloseDoor();

	SetCurrentDoorState(EPMDoorState::EDS_Close);

	OnDoorCloseDelegate.Broadcast(this);
}

void APMDoor::Open()
{
	OnOpen();

	SetCurrentDoorState(EPMDoorState::EDS_OnOpening);

	OnDoorOpeningDelegate.Broadcast(this);
}

void APMDoor::Close()
{
	OnClose();

	SetCurrentDoorState(EPMDoorState::EDS_OnClosing);

	OnDoorClosingDelegate.Broadcast(this);
}

