// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Movement/MovementMode/PMMovement_Walk.h"

UPMMovement_Walk::UPMMovement_Walk(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

bool UPMMovement_Walk::PhysMovement_Implementation(float DeltaTime, int32 Iterations, const APawn* MoveComp)
{
	return true;
}

bool UPMMovement_Walk::StartMovement_Implementation(float DeltaTime, const APawn* MoveComp)
{
	return true;
}
