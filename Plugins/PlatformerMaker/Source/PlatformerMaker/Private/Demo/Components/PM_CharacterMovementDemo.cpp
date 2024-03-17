// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/Components/PM_CharacterMovementDemo.h"
#include "Demo/PM_CharacterDemo.h"
#include "Utils/DebugMacro.h"

const float UPM_CharacterMovementDemo::MIN_TICK_TIME = 1e-6f;
const float UPM_CharacterMovementDemo::MIN_FLOOR_DIST = 1.9f;
const float UPM_CharacterMovementDemo::MAX_FLOOR_DIST = 2.4f;
const float UPM_CharacterMovementDemo::BRAKE_TO_STOP_VELOCITY = 10.f;
const float UPM_CharacterMovementDemo::SWEEP_EDGE_REJECT_DISTANCE = 0.15f;

UPM_CharacterMovementDemo::UPM_CharacterMovementDemo(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	m_acceleration = FPMAccelerationDemo(FVector::ZeroVector, 500.f);
	m_movementMode = EMovementMode::MOVE_None;

	NavAgentProps.bCanWalk = true;
}

void UPM_CharacterMovementDemo::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	if (NewMovementMode == MOVE_Custom) {
		DEBUG_ERROR(TEXT("[%s], do not support Custom Move Mode"), *GetNameSafe(this));
		return;
	}

	// If trying to use NavWalking but there is no navmesh, use walking instead.
	if (NewMovementMode == MOVE_NavWalking) {
		DEBUG_ERROR(TEXT("[%s], do not support Nav walking"), *GetNameSafe(this));
		return;
	}

	// Do nothing if nothing is changing.
	if (m_movementMode == NewMovementMode) {
		FString lEnumString = UEnum::GetValueAsString(m_movementMode);
		DEBUG_WARNING(TEXT("[%s], AllReady in mode [%s]"), *GetNameSafe(this), *lEnumString);
		return;
	}

	m_movementMode = NewMovementMode;

	OnMovementModeUpdated(m_movementMode);
}


void UPM_CharacterMovementDemo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	FVector InputVector = FVector::ZeroVector;

	if (IsValidToMove()) {
		InputVector = ConsumeInputVector();
		CharacterControlledInput(InputVector, DeltaTime);
	}
}

bool UPM_CharacterMovementDemo::IsValidToMove()
{
	return IsValid(GetController()) && IsValid(UpdatedComponent) && m_movementMode != EMovementMode::MOVE_None;
}

void UPM_CharacterMovementDemo::CharacterControlledInput(const FVector& InputVector, float DeltaSeconds)
{
	m_acceleration.AccumulateAccelerationRef(m_acceleration.GetMaxAcceleration() * InputVector.GetClampedToMaxSize(1.0f));
	PerformMovement(DeltaSeconds);
}

FVector UPM_CharacterMovementDemo::ContraintInputZ(FVector& InputVector)
{
	if (InputVector.Z != 0.f && (IsMovingOnGround() || IsFalling()))
	{
		return FVector(InputVector.X, InputVector.Y, 0.f);
	}

	return InputVector;
}

void UPM_CharacterMovementDemo::PerformMovement(float DeltaSeconds)
{
	if (!IsMovingOnGround()) {
		if (IsFalling()) {
			//Apply Fall
			return;
		}
	}

	if (m_acceleration.GetAcceleration().IsNearlyZero()) {
		ApplyVelocityBraking(DeltaSeconds, m_braking.GetBreakingFriction(), m_braking.GetBrakingDecelerationWalking());
		return;
	}

	Velocity += m_acceleration.GetAcceleration();
	MoveAlongFloor(Velocity, DeltaSeconds);
}

void UPM_CharacterMovementDemo::OnMovementModeUpdated(EMovementMode NewMoveMode)
{
}

void UPM_CharacterMovementDemo::MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds)
{
}

void UPM_CharacterMovementDemo::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) 
{

}

void UPM_CharacterMovementDemo::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation) const
{
}

void UPM_CharacterMovementDemo::ComputeFloorDist(float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult) const
{

}

bool UPM_CharacterMovementDemo::FloorSweepTest(FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam) const
{
	return false;
}

float UPM_CharacterMovementDemo::GetGravityZ() const
{
	return Super::GetGravityZ() * m_gravity.GetGravityScale();
}

bool UPM_CharacterMovementDemo::IsMovingOnGround() const
{
	return ((m_movementMode == MOVE_Walking) || (m_movementMode == MOVE_NavWalking)) && IsValid(UpdatedComponent);
}

bool UPM_CharacterMovementDemo::IsFalling() const
{
	return (m_movementMode == MOVE_Falling) && IsValid(UpdatedComponent);
}

void UPM_CharacterMovementDemo::PostLoad()
{
	Super::PostLoad();

	m_demoOwner = Cast<APM_CharacterDemo>(PawnOwner);
}

void UPM_CharacterMovementDemo::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);

	m_demoOwner = Cast<APM_CharacterDemo>(PawnOwner);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Struct Acceleration ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void FPMAccelerationDemo::UpdateAcceleration()
{
	m_acceleration.GetClampedToMaxSize(MaxAcceleration);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// END Struct Acceleration ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////