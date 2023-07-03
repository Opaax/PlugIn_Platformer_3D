// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Components/PMInterpToMovementComponent.h"

void UPMInterpToMovementComponent::RestartForReverse()
{
	CurrentDirection = -1;
	CurrentTime = 1.0f;
	bIsWaiting = false;
	bStopped = false;
}

void UPMInterpToMovementComponent::RestartForNormal()
{
	CurrentDirection = 1;
	CurrentTime = 0.0f;
	bIsWaiting = false;
	bStopped = false;
}
