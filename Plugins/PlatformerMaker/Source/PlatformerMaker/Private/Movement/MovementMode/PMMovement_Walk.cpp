// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Movement/MovementMode/PMMovement_Walk.h"
#include "Utils/DebugMacro.h"
#include "Movement/PMCharacterMovement.h"

//Unreal
#include "Engine/ScopedMovementUpdate.h"
#include "Components/CapsuleComponent.h"

UPMMovement_Walk::UPMMovement_Walk(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

bool UPMMovement_Walk::PhysMovement_Implementation(float DeltaTime, int32 Iterations, UPMCharacterMovement* MoveComp)
{
	float lMinTick = UPMCharacterMovement::MIN_TICK_TIME;

	//return true, is just the remaining time
	if (DeltaTime < lMinTick) {
		return true;
	}

	if (!MoveComp) {
		return false;
	}

	SetMoveComp_Internal(MoveComp);

	if (!m_currentMoveComp || !m_currentMoveComp->GetPawnOwner() || (!m_currentMoveComp->GetPawnOwner()->GetController()))
	{
		m_currentMoveComp->GetAcceleration().SetAcceleration(FVector::ZeroVector);
		m_currentMoveComp->SetVelocity(FVector::ZeroVector);
		return true;
	}

	if (!m_currentMoveComp->UpdatedComponent->IsQueryCollisionEnabled())
	{
		m_currentMoveComp->SetMovementTag(Movement_Native_Mode_Fall);
		return;
	}

	m_currentMoveComp->SetJustTeleported(false);
	bool lbCheckedFall = false;
	bool lbTriedLedgeMove = false;
	float lRemainingTime = DeltaTime;

	// Perform the move
	while ((lRemainingTime >= lMinTick) && (Iterations < m_currentMoveComp->GetMaxSimInterations()) && m_currentMoveComp->GetPawnOwner() && (m_currentMoveComp->GetPawnOwner()->GetController())) {
		Iterations++;
		m_currentMoveComp->SetJustTeleported(false);
		const float lTimeTick = m_currentMoveComp->GetSimulationTimeStep(lRemainingTime, Iterations);
		lRemainingTime -= lTimeTick;

		UPrimitiveComponent* const lOldBase = m_currentMoveComp->GetPawnOwner()->GetMovementBase();
		const FVector lPreviousBaseLocation = (lOldBase != NULL) ? lOldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector lOldLocation = m_currentMoveComp->UpdatedComponent->GetComponentLocation();
		const FPMFindFloorResult lOldFloor = m_currentMoveComp->GetCurrentFloor();

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity(m_currentMoveComp->Velocity);
		const FVector lOldVelocity = m_currentMoveComp->Velocity;
		FVector lAcceleration = FVector();

		m_currentMoveComp->GetAcceleration().GetAccelerationRef(lAcceleration);
		lAcceleration.Z = 0.f;

		m_currentMoveComp->CalcVelocity(lTimeTick, m_brakingSettings, false);

		if (m_currentMoveComp->IsFalling()) {
			// Root motion could have put us into Falling.
			// No movement has taken place this movement tick so we pass on full time/past iteration count
			m_currentMoveComp->StartNewPhysics(lRemainingTime + lTimeTick, Iterations - 1);
			return;
		}

		// Compute move parameters
		const FVector lMoveVelocity = m_currentMoveComp->Velocity;
		const FVector lDelta = lTimeTick * lMoveVelocity;
		const bool bZeroDelta = lDelta.IsNearlyZero();
		FPMStepDownResult lStepDownResult;

		if (bZeroDelta) {
			lRemainingTime = 0.f;
		} 
		else {
			// try to move forward
			MoveAlongFloor(lMoveVelocity, lTimeTick, &lStepDownResult);

			if (MoveComp->IsFalling()) {
				// pawn decided to jump up
				const float DesiredDist = lDelta.Size();
				if (DesiredDist > UE_KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
			else if (IsSwimming()) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}

		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// calculate possible alternate movement
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
			if (!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;
			}
		}


		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround())
		{
			// Make velocity reflect actual move
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}

	if (m_currentMoveComp && m_currentMoveComp->IsMovingOnGround()) {
		MaintainHorizontalGroundVelocity(m_currentMoveComp->Velocity);
	}
	DEBUG_ERROR(TEXT("PHYS MOVEMENT"));
	return true;
}

bool UPMMovement_Walk::StartMovement_Implementation(float DeltaTime, const UPMCharacterMovement* MoveComp)
{
	return true;
}

void UPMMovement_Walk::MaintainHorizontalGroundVelocity_Implementation(FVector& Velocity)
{
	if (Velocity.Z != 0.f) {
		if (bMaintainHorizontalGroundVelocity) {
			// Ramp movement already maintained the velocity, so we just want to remove the vertical component.
			Velocity.Z = 0.f;
		} else {
			// Rescale velocity to be horizontal but maintain magnitude of last update.
			Velocity = Velocity.GetSafeNormal2D() * Velocity.Size();
		}
	}
}

void UPMMovement_Walk::MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FPMFindFloorResult& CurrentFloor, FPMStepDownResult* OutStepDownResult)
{
	if (!CurrentFloor.IsWalkableFloor() || !IsValid(m_currentMoveComp)) {
		return;
	}

	// Move along the current floor
	const FVector lDelta = FVector(InVelocity.X, InVelocity.Y, 0.f) * DeltaSeconds;
	FHitResult lHit(1.f);
	FVector lRampVector = ComputeGroundMovementDelta(lDelta, CurrentFloor.HitResult, CurrentFloor.bLineTrace);
	m_currentMoveComp->SafeMoveUpdatedComponent(lRampVector, m_currentMoveComp->UpdatedComponent->GetComponentQuat(), true, lHit);
	float lLastMoveTimeSlice = DeltaSeconds;

	if (lHit.bStartPenetrating) {
		// Allow this hit to be used as an impact we can deflect off, otherwise we do nothing the rest of the update and appear to hitch.
		m_currentMoveComp->HandleImpact(lHit);
		SlideAlongSurface(lDelta, 1.f, lHit.Normal, lHit, true);

		if (lHit.bStartPenetrating) {
			m_currentMoveComp->OnCharacterStuckInGeometry(&lHit);
		}
	}
	else if (lHit.IsValidBlockingHit()) {
		// We impacted something (most likely another ramp, but possibly a barrier).
		float lPercentTimeApplied = lHit.Time;
		if ((lHit.Time > 0.f) && (lHit.Normal.Z > UE_KINDA_SMALL_NUMBER) && IsWalkable(lHit)) {
			// Another walkable ramp.
			const float lInitialPercentRemaining = 1.f - lPercentTimeApplied;
			lRampVector = ComputeGroundMovementDelta(lDelta * lInitialPercentRemaining, lHit, false);
			lLastMoveTimeSlice = lInitialPercentRemaining * lLastMoveTimeSlice;
			m_currentMoveComp->SafeMoveUpdatedComponent(lRampVector, m_currentMoveComp->UpdatedComponent->GetComponentQuat(), true, lHit);

			const float lSecondHitPercent = lHit.Time * lInitialPercentRemaining;
			lPercentTimeApplied = FMath::Clamp(lPercentTimeApplied + lSecondHitPercent, 0.f, 1.f);
		}

		if (lHit.IsValidBlockingHit()){
			if (CanStepUp(lHit) || (m_currentMoveComp->GetPawnOwner()->GetMovementBase() != nullptr && lHit.HitObjectHandle == m_currentMoveComp->GetPawnOwner()->GetMovementBase()->GetOwner())){
				// hit a barrier, try to step up
				const FVector PreStepUpLocation = m_currentMoveComp->UpdatedComponent->GetComponentLocation();
				const FVector GravDir(0.f, 0.f, -1.f);
				if (!StepUp(GravDir, lDelta * (1.f - lPercentTimeApplied), lHit, OutStepDownResult)){
					m_currentMoveComp->HandleImpact(lHit, lLastMoveTimeSlice, lRampVector);
					SlideAlongSurface(lDelta, 1.f - lPercentTimeApplied, lHit.Normal, lHit, true);
				}
				else {
					if (!bMaintainHorizontalGroundVelocity) {
						// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments. Only consider horizontal movement.
						m_currentMoveComp->SetJustTeleported(true);
						const float lStepUpTimeSlice = (1.f - lPercentTimeApplied) * DeltaSeconds;
						if (lStepUpTimeSlice >= UE_KINDA_SMALL_NUMBER) {
							m_currentMoveComp->Velocity = (m_currentMoveComp->UpdatedComponent->GetComponentLocation() - PreStepUpLocation) / lStepUpTimeSlice;
							m_currentMoveComp->Velocity.Z = 0;
						}
					}
				}
			}
			else if (lHit.Component.IsValid() && !lHit.Component.Get()->CanCharacterStepUp(m_currentMoveComp->GetPawnOwner())){
				m_currentMoveComp->HandleImpact(lHit, lLastMoveTimeSlice, lRampVector);
				SlideAlongSurface(lDelta, 1.f - lPercentTimeApplied, lHit.Normal, lHit, true);
			}
		}
	}
}

FVector UPMMovement_Walk::ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const
{
	if (!IsValid(m_currentMoveComp)) {
		return Delta;
	}

	const FVector lFloorNormal = RampHit.ImpactNormal;
	const FVector lContactNormal = RampHit.Normal;

	if (lFloorNormal.Z < (1.f - UE_KINDA_SMALL_NUMBER) && lFloorNormal.Z > UE_KINDA_SMALL_NUMBER && lContactNormal.Z > UE_KINDA_SMALL_NUMBER && !bHitFromLineTrace && IsWalkable(RampHit)) {
		// Compute a vector that moves parallel to the surface, by projecting the horizontal movement direction onto the ramp.
		const float lFloorDotDelta = (lFloorNormal | Delta);
		FVector lRampMovement(Delta.X, Delta.Y, -lFloorDotDelta / lFloorNormal.Z);

		if (bMaintainHorizontalGroundVelocity) {
			return lRampMovement;
		}
		else{
			return lRampMovement.GetSafeNormal() * Delta.Size();
		}
	}

	return Delta;
}

bool UPMMovement_Walk::IsWalkable_Implementation(const FHitResult& Hit) const
{
	if (!Hit.IsValidBlockingHit()) {
		// No hit, or starting in penetration
		return false;
	}

	// Never walk up vertical surfaces.
	if (Hit.ImpactNormal.Z < UE_KINDA_SMALL_NUMBER) {
		return false;
	}

	float TestWalkableZ = m_walkableFloorZ;

	// See if this component overrides the walkable floor z.
	const UPrimitiveComponent* HitComponent = Hit.Component.Get();
	if (HitComponent) {
		const FWalkableSlopeOverride& SlopeOverride = HitComponent->GetWalkableSlopeOverride();
		TestWalkableZ = SlopeOverride.ModifyWalkableFloorZ(TestWalkableZ);
	}

	// Can't walk on this surface if it is too steep.
	if (Hit.ImpactNormal.Z < TestWalkableZ) {
		return false;
	}

	return true;
}

bool UPMMovement_Walk::StepUp(const FVector& InGravDir, const FVector& Delta, const FHitResult& InHit, FPMStepDownResult* OutStepDownResult)
{
	if (!m_currentMoveComp || !CanStepUp(InHit) || m_maxStepHeight <= 0.f) {
		return false;
	}

	const FVector lOldLocation = m_currentMoveComp->UpdatedComponent->GetComponentLocation();
	float lPawnRadius, lPawnHalfHeight;
	m_currentMoveComp->GetCapsuleComponent()->GetScaledCapsuleSize(lPawnRadius, lPawnHalfHeight);

	// Don't bother stepping up if top of capsule is hitting something.
	const float lInitialImpactZ = InHit.ImpactPoint.Z;
	if (lInitialImpactZ > lOldLocation.Z + (lPawnHalfHeight - lPawnRadius)) {
		return false;
	}

	if (InGravDir.IsZero()){
		return false;
	}

	// Gravity should be a normalized direction
	ensure(InGravDir.IsNormalized());

	float lStepTravelUpHeight = m_maxStepHeight;
	float lStepTravelDownHeight = lStepTravelUpHeight;
	const float lStepSideZ = -1.f * FVector::DotProduct(InHit.ImpactNormal, InGravDir);
	float lPawnInitialFloorBaseZ = lOldLocation.Z - lPawnHalfHeight;
	float lPawnFloorPointZ = lPawnInitialFloorBaseZ;

	const FPMFindFloorResult& lFloor = m_currentMoveComp->GetCurrentFloor();

	if (m_currentMoveComp->IsMovingOnGround() && lFloor.IsWalkableFloor()) {
		// Since we float a variable amount off the floor, we need to enforce max step height off the actual point of impact with the floor.
		const float FloorDist = FMath::Max(0.f, lFloor.GetDistanceToFloor());
		lPawnInitialFloorBaseZ -= FloorDist;
		lStepTravelUpHeight = FMath::Max(lStepTravelUpHeight - FloorDist, 0.f);
		lStepTravelDownHeight = (m_maxStepHeight + m_currentMoveComp->MAX_FLOOR_DIST * 2.f);

		const bool bHitVerticalFace = !IsWithinEdgeTolerance(InHit.Location, InHit.ImpactPoint, lPawnRadius);
		if (!lFloor.bLineTrace && !bHitVerticalFace) {
			lPawnFloorPointZ = lFloor.HitResult.ImpactPoint.Z;
		}
		else {
			// Base floor point is the base of the capsule moved down by how far we are hovering over the surface we are hitting.
			lPawnFloorPointZ -= lFloor.FloorDist;
		}
	}

	// Don't step up if the impact is below us, accounting for distance from floor.
	if (lInitialImpactZ <= lPawnInitialFloorBaseZ) {
		return false;
	}

	// Scope our movement updates, and do not apply them until all intermediate moves are completed.
	FScopedMovementUpdate lScopedStepUpMovement(m_currentMoveComp->UpdatedComponent, EScopedUpdate::DeferredUpdates);

	// step up - treat as vertical wall
	FHitResult lSweepUpHit(1.f);
	const FQuat lPawnRotation = m_currentMoveComp->UpdatedComponent->GetComponentQuat();
	m_currentMoveComp->MoveUpdatedComponent(-InGravDir * lStepTravelUpHeight, lPawnRotation, true, &lSweepUpHit);

	if (lSweepUpHit.bStartPenetrating){
		// Undo movement
		lScopedStepUpMovement.RevertMove();
		return false;
	}

	// step fwd
	FHitResult Hit(1.f);
	m_currentMoveComp->MoveUpdatedComponent(Delta, lPawnRotation, true, &Hit);

	// Check result of forward movement
	if (Hit.bBlockingHit) {
		if (Hit.bStartPenetrating) {
			// Undo movement
			lScopedStepUpMovement.RevertMove();
			return false;
		}

		// If we hit something above us and also something ahead of us, we should notify about the upward hit as well.
		// The forward hit will be handled later (in the bSteppedOver case below).
		// In the case of hitting something above but not forward, we are not blocked from moving so we don't need the notification.
		if (lSweepUpHit.bBlockingHit && Hit.bBlockingHit) {
			m_currentMoveComp->HandleImpact(lSweepUpHit);
		}

		// pawn ran into a wall
		m_currentMoveComp->HandleImpact(Hit);
		if (m_currentMoveComp->IsFalling()) {
			return true;
		}

		// adjust and try again
		const float lForwardHitTime = Hit.Time;
		const float lForwardSlideAmount = SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);

		if (m_currentMoveComp->IsFalling()){
			lScopedStepUpMovement.RevertMove();
			return false;
		}

		// If both the forward hit and the deflection got us nowhere, there is no point in this step up.
		if (lForwardHitTime == 0.f && lForwardSlideAmount == 0.f){
			lScopedStepUpMovement.RevertMove();
			return false;
		}
	}

	// Step down
	m_currentMoveComp->MoveUpdatedComponent(InGravDir * lStepTravelDownHeight, m_currentMoveComp->UpdatedComponent->GetComponentQuat(), true, &Hit);

	// If step down was initially penetrating abort the step up
	if (Hit.bStartPenetrating) {
		lScopedStepUpMovement.RevertMove();
		return false;
	}

	FPMStepDownResult lStepDownResult;
	if (Hit.IsValidBlockingHit()) {
		// See if this step sequence would have allowed us to travel higher than our max step height allows.
		const float lDeltaZ = Hit.ImpactPoint.Z - lPawnFloorPointZ;
		if (lDeltaZ > m_maxStepHeight) {
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (too high Height %.3f) up from floor base %f to %f"), DeltaZ, PawnInitialFloorBaseZ, NewLocation.Z);
			lScopedStepUpMovement.RevertMove();
			return false;
		}

		// Reject unwalkable surface normals here.
		if (!IsWalkable(Hit)){
			// Reject if normal opposes movement direction
			const bool bNormalTowardsMe = (Delta | Hit.ImpactNormal) < 0.f;
			if (bNormalTowardsMe){
				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s opposed to movement)"), *Hit.ImpactNormal.ToString());
				lScopedStepUpMovement.RevertMove();
				return false;
			}

			// Also reject if we would end up being higher than our starting location by stepping down.
			// It's fine to step down onto an unwalkable normal below us, we will just slide off. Rejecting those moves would prevent us from being able to walk off the edge.
			if (Hit.Location.Z > lOldLocation.Z){
				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s above old position)"), *Hit.ImpactNormal.ToString());
				lScopedStepUpMovement.RevertMove();
				return false;
			}
		}

		// Reject moves where the downward sweep hit something very close to the edge of the capsule. This maintains consistency with FindFloor as well.
		if (!IsWithinEdgeTolerance(Hit.Location, Hit.ImpactPoint, lPawnRadius)){
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (outside edge tolerance)"));
			lScopedStepUpMovement.RevertMove();
			return false;
		}

		// Don't step up onto invalid surfaces if traveling higher.
		if (lDeltaZ > 0.f && !CanStepUp(Hit)){
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (up onto surface with !CanStepUp())"));
			lScopedStepUpMovement.RevertMove();
			return false;
		}

		// See if we can validate the floor as a result of this step down. In almost all cases this should succeed, and we can avoid computing the floor outside this method.
		if (OutStepDownResult != NULL) {
			FindFloor(m_currentMoveComp->UpdatedComponent->GetComponentLocation(), lStepDownResult.FloorResult, false, &Hit);

			// Reject unwalkable normals if we end up higher than our initial height.
			// It's fine to walk down onto an unwalkable surface, don't reject those moves.
			if (Hit.Location.Z > lOldLocation.Z){
				// We should reject the floor result if we are trying to step up an actual step where we are not able to perch (this is rare).
				// In those cases we should instead abort the step up and try to slide along the stair.
				if (!lStepDownResult.FloorResult.bBlockingHit && lStepSideZ < CharacterMovementConstants::MAX_STEP_SIDE_Z){
					lScopedStepUpMovement.RevertMove();
					return false;
				}
			}

			lStepDownResult.bComputedFloor = true;
		}
	}

	// Copy step down result.
	if (OutStepDownResult != NULL) {
		*OutStepDownResult = lStepDownResult;
	}

	// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments.
	bool lTempJustTeleport = m_currentMoveComp->IsJustTeleported();
	lTempJustTeleport |= !bMaintainHorizontalGroundVelocity;
	m_currentMoveComp->SetJustTeleported(lTempJustTeleport);

	return true;
}

float UPMMovement_Walk::SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact)
{
	if (!Hit.bBlockingHit) {
		return 0.f;
	}

	FVector lNormal(Normal);
	if (m_currentMoveComp && m_currentMoveComp->IsMovingOnGround()) {
		// We don't want to be pushed up an unwalkable surface.
		if (lNormal.Z > 0.f) {
			if (!IsWalkable(Hit)) {
				lNormal = lNormal.GetSafeNormal2D();
			}
		}
		else if (lNormal.Z < -UE_KINDA_SMALL_NUMBER) {
			// Don't push down into the floor when the impact is on the upper portion of the capsule.
			if (m_currentMoveComp->GetCurrentFloor().FloorDist < m_currentMoveComp->MIN_FLOOR_DIST && m_currentMoveComp->GetCurrentFloor().bBlockingHit){
				const FVector lFloorNormal = m_currentMoveComp->GetCurrentFloor().HitResult.Normal;
				const bool bFloorOpposedToMovement = (Delta | lFloorNormal) < 0.f && (lFloorNormal.Z < 1.f - UE_DELTA);
				if (bFloorOpposedToMovement) {
					lNormal = lFloorNormal;
				}

				lNormal = lNormal.GetSafeNormal2D();
			}
		}
	}
	else if (m_currentMoveComp) {
		m_currentMoveComp->SlideAlongSurface(Delta, Time, Normal, Hit, bHandleImpact);
	}

	return 0.f;
}

bool UPMMovement_Walk::CanStepUp(const FHitResult& Hit) const
{
	if (!Hit.IsValidBlockingHit() || !m_currentMoveComp || m_currentMoveComp->IsMovingOnGround()) {
		return false;
	}

	// No component for "fake" hits when we are on a known good base.
	const UPrimitiveComponent* lHitComponent = Hit.Component.Get();
	if (!lHitComponent) {
		return true;
	}

	if (!lHitComponent->CanCharacterStepUp(m_currentMoveComp->GetPawnOwner())) {
		return false;
	}

	// No actor for "fake" hits when we are on a known good base.

	if (!Hit.HitObjectHandle.IsValid()) {
		return true;
	}

	const AActor* lHitActor = Hit.HitObjectHandle.GetManagingActor();
	if (!lHitActor->CanBeBaseForCharacter(m_currentMoveComp->GetPawnOwner())) {
		return false;
	}

	return true;
}

bool UPMMovement_Walk::IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const
{
	if (m_currentMoveComp) {
		return false;
	}

	const float DistFromCenterSq = (TestImpactPoint - CapsuleLocation).SizeSquared2D();
	const float ReducedRadiusSq = FMath::Square(FMath::Max(m_currentMoveComp->SWEEP_EDGE_REJECT_DISTANCE + UE_KINDA_SMALL_NUMBER, CapsuleRadius - m_currentMoveComp->SWEEP_EDGE_REJECT_DISTANCE));
	return DistFromCenterSq < ReducedRadiusSq;
}