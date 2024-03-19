// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/Components/PM_CharacterMovementDemo.h"
#include "Demo/PM_CharacterDemo.h"
#include "Utils/DebugMacro.h"
#include "PlatformerMaker.h"

//Unreal
#include "Components/CapsuleComponent.h"


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
	NavAgentProps.bCanJump = true;
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

	DEBUG_LOG_SCREEN(20, 10, FColor::Red, TEXT("Is Floor Walkable: %s"), m_currentFloor.bWalkableFloor ? *FString("Walkable") : *FString("NotWalkable"));
	DEBUG_LOG_SCREEN(21, 10, FColor::Red, TEXT("Velocity: %s"), *Velocity.ToString());
	DEBUG_LOG_SCREEN(22, 10, FColor::Red, TEXT("Speed: %lf"), Velocity.Length());
	DEBUG_LOG_SCREEN(23, 10, FColor::Red, TEXT("Acceleration: %s"), *m_acceleration.GetAcceleration().ToString());

	if (ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	if (IsValidToMove()) {
		CharacterControlledInput(DeltaTime);
	}
}

bool UPM_CharacterMovementDemo::IsValidToMove()
{
	return IsValid(GetController()) && IsValid(UpdatedComponent) && m_movementMode != EMovementMode::MOVE_None;
}

void UPM_CharacterMovementDemo::CharacterControlledInput(float DeltaSeconds)
{
	m_currentFrameInput = ConsumeInputVector();

	if (IsFalling()) {
		//Set Acceleration for ai control
		m_acceleration.SetAccelerationRef(FVector::ZeroVector);
		PerformBrakingMovement(DeltaSeconds); //We need to decelerate X, Y vel
		
		if (m_jump.IsJumpRequested()) {
			m_jump.SetJumpRequested(false);
		}

		PerformFallMovement(DeltaSeconds);
		return;
	}

	if (m_currentFrameInput.IsNearlyZero()) {
		m_acceleration.SetAccelerationRef(FVector::ZeroVector);
		PerformBrakingMovement(DeltaSeconds);
		PerformWalkingMovement(DeltaSeconds);
		return;
	}

	m_currentFrameInput.Z = 0;
	m_acceleration.AccumulateAccelerationRef(m_acceleration.GetMaxAcceleration() * m_currentFrameInput.GetClampedToMaxSize(1.0f));
	PerformWalkingMovement(DeltaSeconds);
}

FVector UPM_CharacterMovementDemo::ContraintInputZ(FVector& InputVector)
{
	if (InputVector.Z != 0.f && (IsMovingOnGround() || IsFalling()))
	{
		return FVector(InputVector.X, InputVector.Y, 0.f);
	}

	return InputVector;
}

void UPM_CharacterMovementDemo::PerformWalkingMovement(float DeltaSeconds)
{
	if (!IsMovingOnGround()) {
		FindFloor(UpdatedComponent->GetComponentLocation(), m_currentFloor, false);

		if (!m_currentFloor.bWalkableFloor) {
			if (m_movementMode != EMovementMode::MOVE_Falling) {
				SetMovementMode(EMovementMode::MOVE_Falling);
			}

			return;
		}

		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Velocity, UpdatedComponent->GetComponentQuat(), true, Hit);
	}

	Velocity += m_acceleration.GetAcceleration();
	Velocity = Velocity.GetClampedToMaxSize(m_characterInfo.GetMaxSpeed());
	MoveAlongFloor(Velocity, DeltaSeconds);

	FindFloor(UpdatedComponent->GetComponentLocation(), m_currentFloor, false);
}

void UPM_CharacterMovementDemo::PerformFallMovement(float DeltaSeconds)
{
	if (IsMovingOnGround()) {
		return;
	}

	ApplyVelocityGravity(DeltaSeconds);

	if (Velocity.Z > 0) {
		return;
	}

	FindFloor(UpdatedComponent->GetComponentLocation(), m_currentFloor, false);

	if (m_currentFloor.bWalkableFloor) {
		if (m_movementMode != EMovementMode::MOVE_Walking) {
			SetMovementMode(EMovementMode::MOVE_Walking);
		}
	}
}

void UPM_CharacterMovementDemo::PerformBrakingMovement(float DeltaSeconds)
{
	ApplyVelocityBraking(DeltaSeconds, m_braking.GetBreakingFriction(), m_braking.GetBrakingDecelerationWalking());
}

void UPM_CharacterMovementDemo::OnMovementModeUpdated(EMovementMode NewMoveMode)
{
}

void UPM_CharacterMovementDemo::MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds)
{
	if (!m_currentFloor.bWalkableFloor) {
		FindFloor(UpdatedComponent->GetComponentLocation(), m_currentFloor, false);

		if (!IsFalling()) {
			DEBUG_WARNING_CUSTOM_CATEGORY(LogPlatformerPlugin, TEXT("[%s], Try move along floor, not floor found and is not falling"), *GetNameSafe(this));
		}
		return;
	}

	FHitResult Hit(1.f);
	if (!SafeMoveUpdatedComponent(Velocity * DeltaSeconds, UpdatedComponent->GetComponentQuat(), true, Hit)) {
		DEBUG_WARNING_CUSTOM_CATEGORY(LogPlatformerPlugin, TEXT("[%s] Impact, to move"), *GetNameSafe(this));
		//Velocity = FVector(0,0,Velocity.Z); //we hit something that stop us
	}
}

void UPM_CharacterMovementDemo::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) 
{
	if (Velocity.IsZero() || !IsValidToMove() /*|| HasAnimRootMotion()*/ || DeltaTime < MIN_TICK_TIME) {
		return;
	}

	const float lFrictionFactor = FMath::Max(0.f, m_braking.GetBrakingFactor());
	Friction = FMath::Max(0.f, Friction * lFrictionFactor);
	BrakingDeceleration = FMath::Max(0.f, BrakingDeceleration);
	const bool bZeroFriction = (Friction == 0.f);
	const bool bZeroBraking = (BrakingDeceleration == 0.f);

	if (bZeroFriction && bZeroBraking) {
		return;
	}

	const FVector OldVel = Velocity;

	// subdivide braking to get reasonably consistent results at lower frame rates
	// (important for packet loss situations w/ networking)
	float lRemainingTime = DeltaTime;
	float lSixtyFPSSubTick = 1.f / 33.f;
	const float lMaxTimeStep = FMath::Clamp(lSixtyFPSSubTick, 1.0f / 75.0f, 1.0f / 20.0f);

	// Decelerate to brake to a stop
	const FVector lRevAccel = (bZeroBraking ? FVector::ZeroVector : (-BrakingDeceleration * Velocity.GetSafeNormal()));
	while (lRemainingTime >= MIN_TICK_TIME) {
		// Zero friction uses constant deceleration, so no need for iteration.
		const float dt = ((lRemainingTime > lMaxTimeStep && !bZeroFriction) ? FMath::Min(lMaxTimeStep, lRemainingTime * 0.5f) : lRemainingTime);
		lRemainingTime -= dt;

		// apply friction and braking
		Velocity = Velocity + ((-Friction) * Velocity + lRevAccel) * dt;

		// Don't reverse direction
		if ((Velocity | OldVel) <= 0.f) {
			Velocity = FVector::ZeroVector;
			return;
		}
	}

	// Clamp to zero if nearly zero, or if below min threshold and braking.
	const float lVSizeSq = Velocity.SizeSquared();
	if (lVSizeSq <= UE_KINDA_SMALL_NUMBER || (!bZeroBraking && lVSizeSq <= FMath::Square(BRAKE_TO_STOP_VELOCITY))) {
		Velocity = FVector::ZeroVector;
	}
}

void UPM_CharacterMovementDemo::ApplyVelocityGravity(float DeltaTime)
{
	//TODO
	//Manage Air control after
	//Velocity += m_acceleration.GetAcceleration();
	Velocity.Z += GetGravityZ() * m_gravity.GetGravityScale();

	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Velocity * DeltaTime, UpdatedComponent->GetComponentQuat(), true, Hit);
}

void UPM_CharacterMovementDemo::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation) const
{
	//We need to do this because of const function
	UPM_CharacterMovementDemo* lMutableThis = const_cast<UPM_CharacterMovementDemo*>(this);

	if (!IsValid(lMutableThis)) {
		return;
	}

	bool bIsValidToMove = lMutableThis->IsValidToMove();

	// No collision, no floor...
	if (!bIsValidToMove || !UpdatedComponent->IsQueryCollisionEnabled())
	{
		OutFloorResult.Clear();
		return;
	}

	// Increase height check slightly if walking, to prevent floor height adjustment from later invalidating the floor result.
	const float lHeightCheckAdjust = (IsMovingOnGround() ? MAX_FLOOR_DIST + UE_KINDA_SMALL_NUMBER : -MAX_FLOOR_DIST);

	float lFloorSweepTraceDist = FMath::Max(MAX_FLOOR_DIST, m_characterInfo.GetMaxStepHeight() + lHeightCheckAdjust);
	float lFloorLineTraceDist = lFloorSweepTraceDist;
	bool bNeedToValidateFloor = true;

	if (m_characterInfo.GetAlwaysCheckFloor() || !bCanUseCachedLocation || m_characterInfo.GetForceNextFloorCheck() || bJustTeleported) {
		ComputeFloorDist(lFloorLineTraceDist, lFloorSweepTraceDist, OutFloorResult);
	}


	// Sweep floor
	//if (lFloorLineTraceDist > 0.f || lFloorSweepTraceDist > 0.f) {
	//	UPM_CharacterMovementDemo* MutableThis = const_cast<UPM_CharacterMovementDemo*>(this);

	//	if (m_characterInfo.GetAlwaysCheckFloor() || !bCanUseCachedLocation || m_characterInfo.GetForceNextFloorCheck() || bJustTeleported) {
	//		MutableThis->m_characterInfo.SetForceNextFloorCheck(0);
	//		ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, m_demoOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
	//	}
	//	else {
	//		// Force floor check if base has collision disabled or if it does not block us.
	//		UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
	//		const AActor* BaseActor = MovementBase ? MovementBase->GetOwner() : NULL;
	//		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	//		if (MovementBase != NULL)
	//		{
	//			MutableThis->bForceNextFloorCheck = !MovementBase->IsQueryCollisionEnabled()
	//				|| MovementBase->GetCollisionResponseToChannel(CollisionChannel) != ECR_Block
	//				|| MovementBaseUtility::IsDynamicBase(MovementBase);
	//		}

	//		const bool IsActorBasePendingKill = BaseActor && !IsValid(BaseActor);

	//		if (!bForceNextFloorCheck && !IsActorBasePendingKill && MovementBase)
	//		{
	//			//UE_LOG(LogCharacterMovement, Log, TEXT("%s SKIP check for floor"), *CharacterOwner->GetName());
	//			OutFloorResult = CurrentFloor;
	//			bNeedToValidateFloor = false;
	//		}
	//		else
	//		{
	//			MutableThis->bForceNextFloorCheck = false;
	//			ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
	//		}
	//	}
	//}

	//// OutFloorResult.HitResult is now the result of the vertical floor check.
	//// See if we should try to "perch" at this location.
	//if (bNeedToValidateFloor && OutFloorResult.bBlockingHit && !OutFloorResult.bLineTrace)
	//{
	//	const bool bCheckRadius = true;
	//	if (ShouldComputePerchResult(OutFloorResult.HitResult, bCheckRadius))
	//	{
	//		float MaxPerchFloorDist = FMath::Max(MAX_FLOOR_DIST, MaxStepHeight + HeightCheckAdjust);
	//		if (IsMovingOnGround())
	//		{
	//			MaxPerchFloorDist += FMath::Max(0.f, PerchAdditionalHeight);
	//		}

	//		FFindFloorResult PerchFloorResult;
	//		if (ComputePerchResult(GetValidPerchRadius(), OutFloorResult.HitResult, MaxPerchFloorDist, PerchFloorResult))
	//		{
	//			// Don't allow the floor distance adjustment to push us up too high, or we will move beyond the perch distance and fall next time.
	//			const float AvgFloorDist = (MIN_FLOOR_DIST + MAX_FLOOR_DIST) * 0.5f;
	//			const float MoveUpDist = (AvgFloorDist - OutFloorResult.FloorDist);
	//			if (MoveUpDist + PerchFloorResult.FloorDist >= MaxPerchFloorDist)
	//			{
	//				OutFloorResult.FloorDist = AvgFloorDist;
	//			}

	//			// If the regular capsule is on an unwalkable surface but the perched one would allow us to stand, override the normal to be one that is walkable.
	//			if (!OutFloorResult.bWalkableFloor)
	//			{
	//				// Floor distances are used as the distance of the regular capsule to the point of collision, to make sure AdjustFloorHeight() behaves correctly.
	//				OutFloorResult.SetFromLineTrace(PerchFloorResult.HitResult, OutFloorResult.FloorDist, FMath::Max(OutFloorResult.FloorDist, MIN_FLOOR_DIST), true);
	//			}
	//		}
	//		else
	//		{
	//			// We had no floor (or an invalid one because it was unwalkable), and couldn't perch here, so invalidate floor (which will cause us to start falling).
	//			OutFloorResult.bWalkableFloor = false;
	//		}
	//	}
	//}
}

void UPM_CharacterMovementDemo::ComputeFloorDist(float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult) const
{
	//We need to do this because of const function
	UPM_CharacterMovementDemo* lMutableThis = const_cast<UPM_CharacterMovementDemo*>(this);

	if (!IsValid(lMutableThis)) {
		return;
	}

	// We require the sweep distance to be >= the line distance, otherwise the HitResult can't be interpreted as the sweep result.
	if (SweepDistance < LineDistance) {
		ensure(SweepDistance >= LineDistance);
		return;
	}
	const FVector& lCapsuleLocation = m_demoOwner->GetCapsuleComponent()->GetComponentLocation();
	const float lSweepRadius = m_demoOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float PawnRadius, PawnHalfHeight;
	bool bBlockingHit = false;
	FCollisionQueryParams lQueryParams(SCENE_QUERY_STAT(ComputeFloorDist), false, m_demoOwner);
	FCollisionResponseParams lResponseParam;
	InitCollisionParams(lQueryParams, lResponseParam);
	const ECollisionChannel lCollisionChannel = UpdatedComponent->GetCollisionObjectType();

	m_demoOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	if (SweepDistance > 0.f && lSweepRadius > 0.f) {
		// Use a shorter height to avoid sweeps giving weird results if we start on a surface.
		// This also allows us to adjust out of penetrations.
		const float ShrinkScale = 0.9f;
		const float ShrinkScaleOverlap = 0.1f;
		float ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScale);

		float TraceDist = SweepDistance + ShrinkHeight;
		FCollisionShape lCapsuleShape = FCollisionShape::MakeCapsule(lSweepRadius, PawnHalfHeight - ShrinkHeight);
		const FVector lSweepTestTargetLoc = lCapsuleLocation + FVector(0.f, 0.f, -TraceDist);

		DrawDebugCapsule(GetWorld(), lCapsuleLocation, PawnHalfHeight - ShrinkHeight, lSweepRadius, m_demoOwner->GetCapsuleComponent()->GetComponentQuat(), FColor::Green, false, 2.f);
		DrawDebugCapsule(GetWorld(), lSweepTestTargetLoc, PawnHalfHeight - ShrinkHeight, lSweepRadius, m_demoOwner->GetCapsuleComponent()->GetComponentQuat(), FColor::Blue, false, 2.f);

		FHitResult Hit(1.f);
		bBlockingHit = FloorSweepTest(Hit, lCapsuleLocation, lSweepTestTargetLoc, lCollisionChannel, lCapsuleShape, lQueryParams, lResponseParam);

		if (bBlockingHit) {
			// Reject hits adjacent to us, we only care about hits on the bottom portion of our capsule.
			// Check 2D distance to impact point, reject if within a tolerance from radius.
			if (Hit.bStartPenetrating /*|| !IsWithinEdgeTolerance(CapsuleLocation, Hit.ImpactPoint, CapsuleShape.Capsule.Radius)*/) {
				// Use a capsule with a slightly smaller radius and shorter height to avoid the adjacent object.
				// Capsule must not be nearly zero or the trace will fall back to a line trace from the start point and have the wrong length.
				lCapsuleShape.Capsule.Radius = FMath::Max(0.f, lCapsuleShape.Capsule.Radius - SWEEP_EDGE_REJECT_DISTANCE - UE_KINDA_SMALL_NUMBER);
				if (!lCapsuleShape.IsNearlyZero())
				{
					ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScaleOverlap);
					TraceDist = SweepDistance + ShrinkHeight;
					lCapsuleShape.Capsule.HalfHeight = FMath::Max(PawnHalfHeight - ShrinkHeight, lCapsuleShape.Capsule.Radius);
					Hit.Reset(1.f, false);

					bBlockingHit = FloorSweepTest(Hit, lCapsuleLocation, lSweepTestTargetLoc, lCollisionChannel, lCapsuleShape, lQueryParams, lResponseParam);
				}
			}

			// Reduce hit distance by ShrinkHeight because we shrank the capsule for the trace.
			// We allow negative distances here, because this allows us to pull out of penetrations.
			const float MaxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, PawnRadius);
			const float SweepResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

			OutFloorResult.SetFromSweep(Hit, SweepResult, false);
			if (Hit.IsValidBlockingHit() /*&& IsWalkable(Hit)*/)
			{
				if (SweepResult <= SweepDistance)
				{
					// Hit within test distance.
					OutFloorResult.bWalkableFloor = true;
					if (!lMutableThis->IsMovingOnGround()) {
						lMutableThis->SetMovementMode(EMovementMode::MOVE_Walking);
					}

					return;
				}
			}
		}
	}

	OutFloorResult.bWalkableFloor = false;
	OutFloorResult.Clear();

	if (!lMutableThis->IsFalling()) {
		lMutableThis->SetMovementMode(EMovementMode::MOVE_Falling);
	}
}

bool UPM_CharacterMovementDemo::FloorSweepTest(FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam) const
{
	bool bBlockingHit = false;
	
	if (!m_characterInfo.GetUseFlatBaseForFloorChecks()) {
		bBlockingHit = GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, TraceChannel, CollisionShape, Params, ResponseParam);
	}
	else {
		// Test with a box that is enclosed by the capsule.
		const float CapsuleRadius = CollisionShape.GetCapsuleRadius();
		const float CapsuleHeight = CollisionShape.GetCapsuleHalfHeight();
		const FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(CapsuleRadius * 0.707f, CapsuleRadius * 0.707f, CapsuleHeight));
	
		// First test with the box rotated so the corners are along the major axes (ie rotated 45 degrees).
		bBlockingHit = GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat(FVector(0.f, 0.f, -1.f), UE_PI * 0.25f), TraceChannel, BoxShape, Params, ResponseParam);
	
		if (!bBlockingHit) {
			// Test again with the same box, not rotated.
			OutHit.Reset(1.f, false);
			bBlockingHit = GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, TraceChannel, BoxShape, Params, ResponseParam);
		}
	}
	
	return bBlockingHit;
}

bool UPM_CharacterMovementDemo::Jump()
{
	if (IsValid(m_demoOwner) && CanJump()) {
		m_jump.SetJumpRequested(true);
		Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, m_jump.GetJumpForce());

		SetMovementMode(MOVE_Falling);

		return true;
	}

	return false;
}

bool UPM_CharacterMovementDemo::CanJump()
{
	return NavAgentProps.bCanJump;
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

float UPM_CharacterMovementDemo::GetMaxSpeed() const
{
	return m_characterInfo.GetMaxSpeed();
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
	m_acceleration = m_acceleration.GetClampedToMaxSize(MaxAcceleration);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// END Struct Acceleration ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////