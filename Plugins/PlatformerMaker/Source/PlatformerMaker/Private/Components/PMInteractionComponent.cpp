// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Components/PMInteractionComponent.h"

// Sets default values for this component's properties
UPMInteractionComponent::UPMInteractionComponent(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPMInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPMInteractionComponent::StartInteraction()
{
	if (!bInteractable)
	{
		return;
	}

	SetActivated(true);

	OnInteract.Broadcast(bActivated);

	ReceiveStartInteraction();
}

void UPMInteractionComponent::StopInteraction()
{
	if (!bInteractable)
	{
		return;
	}

	SetActivated(false);

	OnInteract.Broadcast(bActivated);

	ReceiveStopInteraction();
}

void UPMInteractionComponent::EnableInteraction()
{
	bInteractable = true;

	OnInteractionToggled.Broadcast(this, bInteractable);

	ReceiveEnableInteraction();
}

void UPMInteractionComponent::DisableInteraction()
{
	bInteractable = false;

	OnInteractionToggled.Broadcast(this, bInteractable);

	ReceiveDisableInteraction();
}

