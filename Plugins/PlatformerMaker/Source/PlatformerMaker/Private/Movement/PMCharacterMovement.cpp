// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Movement/PMCharacterMovement.h"

UPMCharacterMovement::UPMCharacterMovement(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

void UPMCharacterMovement::PerformMovement_Implementation(float DeltaTime)
{
   
}

void UPMCharacterMovement::StartNewPhysics_Implementation(float DeltaTime, int32 Interation)
{
}

void UPMCharacterMovement::OnMoveTagChanged_Implementation()
{
}

bool UPMCharacterMovement::SetMovementTag(const FGameplayTag& NewMoveTag)
{
	bool lHasChangeTag = false;

	m_currentMovementTag = NewMoveTag;
	OnMoveTagChanged();

	return lHasChangeTag;
}

/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
/*------------------------------------- OVERRIDES -------------------------------------*/
/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

void UPMCharacterMovement::PostLoad()
{
	Super::PostLoad();
}

float UPMCharacterMovement::GetGravityZ() const
{
	return Super::GetGravityZ();
}

bool UPMCharacterMovement::IsMovingOnGround() const
{
	return false;
}

bool UPMCharacterMovement::IsFalling() const
{
	return false;
}

float UPMCharacterMovement::GetMaxSpeed() const
{
	return 0;
}

void UPMCharacterMovement::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);
}

void UPMCharacterMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
