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
}

void APMDoor::OnClose()
{
	ReceiveOnClose();
}

void APMDoor::OnFinishCloseDoor()
{
	m_currentStateDoor = EPMDoorState::EDS_Close;

	OnDoorCloseDelegate.Broadcast(this);

	ReceiveOnFinishCloseDoor();
}

void APMDoor::Open()
{
	OnOpen();

	m_currentStateDoor = EPMDoorState::EDS_OnOpening;

	OnDoorOpeningDelegate.Broadcast(this);

	UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, Door Open"), *GetName());
}

void APMDoor::Close()
{
	OnClose();

	m_currentStateDoor = EPMDoorState::EDS_OnClosing;

	OnDoorClosingDelegate.Broadcast(this);

	UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, Door Close"), *GetName());
}

