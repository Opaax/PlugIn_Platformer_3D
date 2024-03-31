// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Movement/PMMovementMode.h"
#include "Movement/PMCharacterMovement.h"

UPMMovementMode::UPMMovementMode(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

void UPMMovementMode::SetMoveComp_Internal(UPMCharacterMovement* InMoveComp)
{
	if (IsValid(m_currentMoveComp) && m_currentMoveComp != InMoveComp) {
		m_currentMoveComp = InMoveComp;
		return;
	}

	m_currentMoveComp = InMoveComp;
}