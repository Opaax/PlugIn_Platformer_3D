// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Movement/PMCharacterMovement.h"
#include "Movement/PMMovementMode.h"

//Unreal
#include "Engine/ScopedMovementUpdate.h"
#include "GameFramework/Character.h" //Need it for namespace utility
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

//TAGS
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Mode_Walking, "Movement.Native.Mode.Walking");
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Mode_Crouch, "Movement.Native.Mode.Crouch");
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Mode_Fall, "Movement.Native.Mode.Fall");
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Mode_Jump, "Movement.Native.Mode.Jump");

UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Sate_WantsCrouch, "Movement.Native.Sate.WantsCrouch");
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Sate_IsCrouch, "Movement.Native.Sate.IsCrouch");

UPMCharacterMovement::UPMCharacterMovement(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

bool UPMCharacterMovement::IsValidToMove()
{
	return IsValid(GetController()) && IsValidBaseData() && IsValid(m_currentMovement) && m_currentMovementTag.IsValid();
}

bool UPMCharacterMovement::IsValidBaseData()
{
	return UpdatedComponent && IsValid(m_capsuleComponent) && IsValid(GetPawnOwner());
}

void UPMCharacterMovement::OnUnableToFollowBaseMove_Implementation(const FVector& DeltaPosition, const FVector& OldLocation, const FHitResult& MoveOnBaseHit)
{
	//as UE character movement let inherited to change, but with BP possibility
}

void UPMCharacterMovement::PrePhysicsTickComponent(float DeltaTime, FPMCharacterMovementPrePhysicsTickFunction& ThisTickFunction)
{
	//Debug after @see Character movement component
}

void UPMCharacterMovement::PostPhysicsTickComponent(float DeltaTime, FPMCharacterMovementPostPhysicsTickFunction& ThisTickFunction)
{
	//DEBUG @see Character movement component

	if (bDeferUpdateBasedMovement) {
		//DEBUG @see Character movement component

		FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);
		UpdateBasedMovement(DeltaTime);
		bDeferUpdateBasedMovement = false;
	}

}

void UPMCharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	//@See UE Character movement
}

void UPMCharacterMovement::CallMovementUpdateDelegate(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	// Update component velocity in case events want to read it
	UpdateComponentVelocity();

	// Delegate (for blueprints)
	if (IsValidBaseData()) {
		OnMovementUpdatedDelegate.Broadcast(GetPawnOwner(), DeltaSeconds, OldLocation, OldVelocity);
	}
}

void UPMCharacterMovement::UpdateBasedMovement(float DeltaSeconds)
{
	if (!IsValidBaseData()) {
		return;
	}

	const UPrimitiveComponent* lMovementBase = GetPawnOwner()->GetMovementBase();

	if (!MovementBaseUtility::UseRelativeLocation(lMovementBase)) {
		return;
	}

	if (!IsValid(lMovementBase) || !IsValid(lMovementBase->GetOwner())) {
		return;
	}

	// Ignore collision with bases during these movements.
	TGuardValue<EMoveComponentFlags> ScopedFlagRestore(MoveComponentFlags, MoveComponentFlags | MOVECOMP_IgnoreBases);

	FQuat DeltaQuat = FQuat::Identity;
	FVector DeltaPosition = FVector::ZeroVector;

	FQuat NewBaseQuat = lMovementBase->GetComponentQuat();
	FVector NewBaseLocation = lMovementBase->GetComponentLocation();

	// Find change in rotation
	const bool lbRotationChanged = !m_oldBaseQuat.Equals(NewBaseQuat, 1e-8f);
	if (lbRotationChanged) {
		DeltaQuat = NewBaseQuat * m_oldBaseQuat.Inverse();
	}

	// only if base moved
	if (lbRotationChanged || (m_oldBaseLocation != NewBaseLocation)) {
		// Calculate new transform matrix of base actor (ignoring scale).
		const FQuatRotationTranslationMatrix OldLocalToWorld(m_oldBaseQuat, m_oldBaseLocation);
		const FQuatRotationTranslationMatrix NewLocalToWorld(NewBaseQuat, NewBaseLocation);

		FQuat lFinalQuat = UpdatedComponent->GetComponentQuat();

		if (lbRotationChanged && !bIgnoreBaseRotation) {
			// Apply change in rotation and pipe through FaceRotation to maintain axis restrictions
			const FQuat lPawnOldQuat = UpdatedComponent->GetComponentQuat();
			const FQuat lTargetQuat = DeltaQuat * lFinalQuat;
			FRotator lTargetRotator(lTargetQuat);
			GetPawnOwner()->FaceRotation(lTargetRotator, 0.f);
			lFinalQuat = UpdatedComponent->GetComponentQuat();

			if (lPawnOldQuat.Equals(lFinalQuat, 1e-6f)) {
				// Nothing changed. This means we probably are using another rotation mechanism (bOrientToMovement etc). We should still follow the base object.
				// @todo: This assumes only Yaw is used, currently a valid assumption. This is the only reason FaceRotation() is used above really, aside from being a virtual hook.
				if (bOrientRotationToMovement || (bUseControllerDesiredRotation && GetController())) {
					lTargetRotator.Pitch = 0.f;
					lTargetRotator.Roll = 0.f;
					MoveUpdatedComponent(FVector::ZeroVector, lTargetRotator, false);
					lFinalQuat = UpdatedComponent->GetComponentQuat();
				}
			}

			// Pipe through ControlRotation, to affect camera.
			if (GetController()) {
				const FQuat lPawnDeltaRotation = lFinalQuat * lPawnOldQuat.Inverse();
				FRotator lFinalRotation = lFinalQuat.Rotator();
				UpdateBasedRotation(lFinalRotation, lPawnDeltaRotation.Rotator());
				lFinalQuat = UpdatedComponent->GetComponentQuat();
			}
		}

		// We need to offset the base of the character here, not its origin, so offset by half height
		float lHalfHeight, lRadius;
		m_capsuleComponent->GetScaledCapsuleSize(lRadius, lHalfHeight); //check in IsValidBaseData

		FVector const lBaseOffset(0.0f, 0.0f, lHalfHeight);
		FVector const lLocalBasePos = OldLocalToWorld.InverseTransformPosition(UpdatedComponent->GetComponentLocation() - lBaseOffset);
		FVector const lNewWorldPos = ConstrainLocationToPlane(NewLocalToWorld.TransformPosition(lLocalBasePos) + lBaseOffset);
		DeltaPosition = ConstrainDirectionToPlane(lNewWorldPos - UpdatedComponent->GetComponentLocation());

		// move attached actor
		if (bFastAttachedMove) {
			// we're trusting no other obstacle can prevent the move here
			UpdatedComponent->SetWorldLocationAndRotation(lNewWorldPos, lFinalQuat, false);
		}
		else {
			// hack - transforms between local and world space introducing slight error FIXMESTEVE - discuss with engine team: just skip the transforms if no rotation?
			FVector lBaseMoveDelta = NewBaseLocation - m_oldBaseLocation;
			if (!lbRotationChanged && (lBaseMoveDelta.X == 0.f) && (lBaseMoveDelta.Y == 0.f)) {
				DeltaPosition.X = 0.f;
				DeltaPosition.Y = 0.f;
			}

			FHitResult MoveOnBaseHit(1.f);
			const FVector OldLocation = UpdatedComponent->GetComponentLocation();
			MoveUpdatedComponent(DeltaPosition, lFinalQuat, true, &MoveOnBaseHit);
			if ((UpdatedComponent->GetComponentLocation() - (OldLocation + DeltaPosition)).IsNearlyZero() == false) {
				OnUnableToFollowBaseMove(DeltaPosition, OldLocation, MoveOnBaseHit);
			}
		}

		if (lMovementBase->IsSimulatingPhysics() && IsValid(m_ownerMesh)) {
			m_ownerMesh->ApplyDeltaToAllPhysicsTransforms(DeltaPosition, DeltaQuat);
		}
	}
}

void UPMCharacterMovement::MaybeUpdateBasedMovement(float DeltaSeconds)
{
	bDeferUpdateBasedMovement = false;

	UPrimitiveComponent* lMovementBase = GetPawnOwner()->GetMovementBase();
	if (MovementBaseUtility::UseRelativeLocation(lMovementBase)) {
		// Need to see if anything we're on is simulating physics or has a parent that is.		
		if (!MovementBaseUtility::IsSimulatedBase(lMovementBase)) {
			bDeferUpdateBasedMovement = false;
			UpdateBasedMovement(DeltaSeconds);
			// If previously simulated, go back to using normal tick dependencies.
			if (PostPhysicsTickFunction.IsTickFunctionEnabled()) {
				PostPhysicsTickFunction.SetTickFunctionEnable(false);
				MovementBaseUtility::AddTickDependency(PrimaryComponentTick, lMovementBase);
			}
		}
		else {
			// defer movement base update until after physics
			bDeferUpdateBasedMovement = true;
			// If previously not simulating, remove tick dependencies and use post physics tick function.
			if (!PostPhysicsTickFunction.IsTickFunctionEnabled()) {
				PostPhysicsTickFunction.SetTickFunctionEnable(true);
				MovementBaseUtility::RemoveTickDependency(PrimaryComponentTick, lMovementBase);
			}
		}
	}
	else {
		// Remove any previous physics tick dependencies. SetBase() takes care of the other dependencies.
		if (PostPhysicsTickFunction.IsTickFunctionEnabled()) {
			PostPhysicsTickFunction.SetTickFunctionEnable(false);
		}
	}
}

void UPMCharacterMovement::UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation)
{
	AController* lController = nullptr;

	if (IsValidBaseData()) {
		lController = GetPawnOwner()->GetController();
	}

	float lControllerRoll = 0.f;

	if ((lController != nullptr) && !bIgnoreBaseRotation) {
		FRotator lControllerRot = lController->GetControlRotation();
		lControllerRoll = lControllerRot.Roll;
		lController->SetControlRotation(lControllerRot + ReducedRotation);
	}

	// Remove roll
	FinalRotation.Roll = 0.f;
	if (lController != nullptr) {
		FinalRotation.Roll = UpdatedComponent->GetComponentRotation().Roll;
		FRotator lNewRotation = lController->GetControlRotation();
		lNewRotation.Roll = lControllerRoll;
		lController->SetControlRotation(lNewRotation);
	}
}

void UPMCharacterMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	//TODO
	//Check crouch state here
}

void UPMCharacterMovement::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	//Todo
	//recheck if the new state can allow crouch
}

bool UPMCharacterMovement::HandlePendingLaunch()
{
	if (!m_pendingLaunchVelocity.IsZero() && IsValidBaseData()){
		Velocity = m_pendingLaunchVelocity;
		SetMovementTag(Movement_Native_Mode_Fall);
		m_pendingLaunchVelocity = FVector::ZeroVector;
		bForceNextFloorCheck = true;

		return true;
	}

	return false;
}

void UPMCharacterMovement::PerformMovement_Implementation(float DeltaTime)
{
	const UWorld* lWorld = GetWorld();
	if (!IsValidToMove() || lWorld == nullptr) {
		return;
	}

	bTeleportedSinceLastUpdate = UpdatedComponent->GetComponentLocation() != m_lastUpdateLocation;

	// no movement if we can't move, or if currently doing physical simulation on UpdatedComponent
	if (!m_currentMovementTag.IsValid() || UpdatedComponent->Mobility != EComponentMobility::Movable || UpdatedComponent->IsSimulatingPhysics())
	{
		//Root motioin for later
		/*
		//if (!CharacterOwner->bClientUpdating && !CharacterOwner->bServerMoveIgnoreRootMotion)
		//{
		//	// Consume root motion
		//	if (CharacterOwner->IsPlayingRootMotion() && CharacterOwner->GetMesh())
		//	{
		//		TickCharacterPose(DeltaSeconds);
		//		RootMotionParams.Clear();
		//	}
		//	if (CurrentRootMotion.HasActiveRootMotionSources())
		//	{
		//		CurrentRootMotion.Clear();
		//	}
		//}*/

		// Clear pending physics forces
		ClearAccumulatedForces();

		return;
	}

	// Force floor update if we've moved outside of CharacterMovement since last update.
	bForceNextFloorCheck |= (IsMovingOnGround() && bTeleportedSinceLastUpdate);

	//Root motion for later
	/*
	// Update saved LastPreAdditiveVelocity with any external changes to character Velocity that happened since last update.
	if (CurrentRootMotion.HasAdditiveVelocity())
	{
		const FVector Adjustment = (Velocity - LastUpdateVelocity);
		CurrentRootMotion.LastPreAdditiveVelocity += Adjustment;

#if ROOT_MOTION_DEBUG
		if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
		{
			if (!Adjustment.IsNearlyZero())
			{
				FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement HasAdditiveVelocity LastUpdateVelocityAdjustment LastPreAdditiveVelocity(%s) Adjustment(%s)"),
					*CurrentRootMotion.LastPreAdditiveVelocity.ToCompactString(), *Adjustment.ToCompactString());
				RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
			}
		}
#endif
	}
	*/
	FVector lOldVelocity;
	FVector lOldLocation;

	//Unreal advice
	// Scoped updates can improve performance of multiple MoveComponent calls.
	{
		FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);

		MaybeUpdateBasedMovement(DeltaTime);

		//Root motion
		/*
		// Clean up invalid RootMotion Sources.
		// This includes RootMotion sources that ended naturally.
		// They might want to perform a clamp on velocity or an override, 
		// so we want this to happen before ApplyAccumulatedForces and HandlePendingLaunch as to not clobber these.
		const bool bHasRootMotionSources = HasRootMotionSources();
		if (bHasRootMotionSources && !CharacterOwner->bClientUpdating && !CharacterOwner->bServerMoveIgnoreRootMotion)
		{
			SCOPE_CYCLE_COUNTER(STAT_CharacterMovementRootMotionSourceCalculate);

			const FVector VelocityBeforeCleanup = Velocity;
			CurrentRootMotion.CleanUpInvalidRootMotion(DeltaSeconds, *CharacterOwner, *this);

#if ROOT_MOTION_DEBUG
			if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
			{
				if (Velocity != VelocityBeforeCleanup)
				{
					const FVector Adjustment = Velocity - VelocityBeforeCleanup;
					FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement CleanUpInvalidRootMotion Velocity(%s) VelocityBeforeCleanup(%s) Adjustment(%s)"),
						*Velocity.ToCompactString(), *VelocityBeforeCleanup.ToCompactString(), *Adjustment.ToCompactString());
					RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
				}
			}
#endif
		}*/

		lOldVelocity = Velocity;
		lOldLocation = UpdatedComponent->GetComponentLocation();

		ApplyAccumulatedForces(DeltaTime);

		// Update the character state before we do our movement
		UpdateCharacterStateBeforeMovement(DeltaTime);

		/*if (MovementMode == MOVE_NavWalking && bWantsToLeaveNavWalking)
		{
			TryToLeaveNavWalking();
		}*/

		// Character::LaunchCharacter() has been deferred until now.
		HandlePendingLaunch();
		ClearAccumulatedForces();
/*
#if ROOT_MOTION_DEBUG
		if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
		{
			if (OldVelocity != Velocity)
			{
				const FVector Adjustment = Velocity - OldVelocity;
				FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement ApplyAccumulatedForces+HandlePendingLaunch Velocity(%s) OldVelocity(%s) Adjustment(%s)"),
					*Velocity.ToCompactString(), *OldVelocity.ToCompactString(), *Adjustment.ToCompactString());
				RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
			}
		}
#endif*/
		
		/*
		// Update saved LastPreAdditiveVelocity with any external changes to character Velocity that happened due to ApplyAccumulatedForces/HandlePendingLaunch
		if (CurrentRootMotion.HasAdditiveVelocity())
		{
			const FVector Adjustment = (Velocity - OldVelocity);
			CurrentRootMotion.LastPreAdditiveVelocity += Adjustment;

#if ROOT_MOTION_DEBUG
			if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
			{
				if (!Adjustment.IsNearlyZero())
				{
					FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement HasAdditiveVelocity AccumulatedForces LastPreAdditiveVelocity(%s) Adjustment(%s)"),
						*CurrentRootMotion.LastPreAdditiveVelocity.ToCompactString(), *Adjustment.ToCompactString());
					RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
				}
			}
#endif
		}

		// Prepare Root Motion (generate/accumulate from root motion sources to be used later)
		if (bHasRootMotionSources && !CharacterOwner->bClientUpdating && !CharacterOwner->bServerMoveIgnoreRootMotion)
		{
			// Animation root motion - If using animation RootMotion, tick animations before running physics.
			if (CharacterOwner->IsPlayingRootMotion() && CharacterOwner->GetMesh())
			{
				TickCharacterPose(DeltaSeconds);

				// Make sure animation didn't trigger an event that destroyed us
				if (!HasValidData())
				{
					return;
				}

				// For local human clients, save off root motion data so it can be used by movement networking code.
				if (CharacterOwner->IsLocallyControlled() && (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy) && CharacterOwner->IsPlayingNetworkedRootMotionMontage())
				{
					CharacterOwner->ClientRootMotionParams = RootMotionParams;
				}
			}

			// Generates root motion to be used this frame from sources other than animation
			{
				SCOPE_CYCLE_COUNTER(STAT_CharacterMovementRootMotionSourceCalculate);
				CurrentRootMotion.PrepareRootMotion(DeltaSeconds, *CharacterOwner, *this, true);
			}

			// For local human clients, save off root motion data so it can be used by movement networking code.
			if (CharacterOwner->IsLocallyControlled() && (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy))
			{
				CharacterOwner->SavedRootMotion = CurrentRootMotion;
			}
		}

		// Apply Root Motion to Velocity
		if (CurrentRootMotion.HasOverrideVelocity() || HasAnimRootMotion())
		{
			// Animation root motion overrides Velocity and currently doesn't allow any other root motion sources
			if (HasAnimRootMotion())
			{
				// Convert to world space (animation root motion is always local)
				USkeletalMeshComponent* SkelMeshComp = CharacterOwner->GetMesh();
				if (SkelMeshComp)
				{
					// Convert Local Space Root Motion to world space. Do it right before used by physics to make sure we use up to date transforms, as translation is relative to rotation.
					RootMotionParams.Set(ConvertLocalRootMotionToWorld(RootMotionParams.GetRootMotionTransform(), DeltaSeconds));
				}

				// Then turn root motion to velocity to be used by various physics modes.
				if (DeltaSeconds > 0.f)
				{
					AnimRootMotionVelocity = CalcAnimRootMotionVelocity(RootMotionParams.GetRootMotionTransform().GetTranslation(), DeltaSeconds, Velocity);
					Velocity = ConstrainAnimRootMotionVelocity(AnimRootMotionVelocity, Velocity);
					if (IsFalling())
					{
						Velocity += FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f);
					}
				}

				UE_LOG(LogRootMotion, Log, TEXT("PerformMovement WorldSpaceRootMotion Translation: %s, Rotation: %s, Actor Facing: %s, Velocity: %s")
					, *RootMotionParams.GetRootMotionTransform().GetTranslation().ToCompactString()
					, *RootMotionParams.GetRootMotionTransform().GetRotation().Rotator().ToCompactString()
					, *CharacterOwner->GetActorForwardVector().ToCompactString()
					, *Velocity.ToCompactString()
				);
			}
			else
			{
				// We don't have animation root motion so we apply other sources
				if (DeltaSeconds > 0.f)
				{
					SCOPE_CYCLE_COUNTER(STAT_CharacterMovementRootMotionSourceApply);

					const FVector VelocityBeforeOverride = Velocity;
					FVector NewVelocity = Velocity;
					CurrentRootMotion.AccumulateOverrideRootMotionVelocity(DeltaSeconds, *CharacterOwner, *this, NewVelocity);
					if (IsFalling())
					{
						NewVelocity += CurrentRootMotion.HasOverrideVelocityWithIgnoreZAccumulate() ? FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f) : DecayingFormerBaseVelocity;
					}
					Velocity = NewVelocity;

#if ROOT_MOTION_DEBUG
					if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
					{
						if (VelocityBeforeOverride != Velocity)
						{
							FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement AccumulateOverrideRootMotionVelocity Velocity(%s) VelocityBeforeOverride(%s)"),
								*Velocity.ToCompactString(), *VelocityBeforeOverride.ToCompactString());
							RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
						}
					}
#endif
				}
			}
		}*/

/*
#if ROOT_MOTION_DEBUG
		if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
		{
			FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement Velocity(%s) OldVelocity(%s)"),
				*Velocity.ToCompactString(), *OldVelocity.ToCompactString());
			RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
		}
#endif*/

		// NaN tracking
		//devCode(ensureMsgf(!Velocity.ContainsNaN(), TEXT("UCharacterMovementComponent::PerformMovement: Velocity contains NaN (%s)\n%s"), *GetPathNameSafe(this), *Velocity.ToString()));

		//TODO Check Jump
		// Clear jump input now, to allow movement events to trigger it for next update.
		//CharacterOwner->ClearJumpInput(DeltaSeconds);
		//NumJumpApexAttempts = 0;

		// change position
		StartNewPhysics(DeltaTime, 0);

		if (!IsValidBaseData()) {
			return;
		}

		// Update character state based on change from movement
		UpdateCharacterStateAfterMovement(DeltaTime);

		//RootMOtion
		/*if (bAllowPhysicsRotationDuringAnimRootMotion || !HasAnimRootMotion())
		{
			PhysicsRotation(DeltaSeconds);
		}*/
		/*
//		// Apply Root Motion rotation after movement is complete.
//		if (HasAnimRootMotion())
//		{
//			const FQuat OldActorRotationQuat = UpdatedComponent->GetComponentQuat();
//			const FQuat RootMotionRotationQuat = RootMotionParams.GetRootMotionTransform().GetRotation();
//			if (!RootMotionRotationQuat.IsIdentity())
//			{
//				const FQuat NewActorRotationQuat = RootMotionRotationQuat * OldActorRotationQuat;
//				MoveUpdatedComponent(FVector::ZeroVector, NewActorRotationQuat, true);
//			}
//
//#if !(UE_BUILD_SHIPPING)
//			// debug
//			if (false)
//			{
//				const FRotator OldActorRotation = OldActorRotationQuat.Rotator();
//				const FVector ResultingLocation = UpdatedComponent->GetComponentLocation();
//				const FRotator ResultingRotation = UpdatedComponent->GetComponentRotation();
//
//				// Show current position
//				DrawDebugCoordinateSystem(MyWorld, CharacterOwner->GetMesh()->GetComponentLocation() + FVector(0, 0, 1), ResultingRotation, 50.f, false);
//
//				// Show resulting delta move.
//				DrawDebugLine(MyWorld, OldLocation, ResultingLocation, FColor::Red, false, 10.f);
//
//				// Log details.
//				UE_LOG(LogRootMotion, Warning, TEXT("PerformMovement Resulting DeltaMove Translation: %s, Rotation: %s, MovementBase: %s"), //-V595
//					*(ResultingLocation - OldLocation).ToCompactString(), *(ResultingRotation - OldActorRotation).GetNormalized().ToCompactString(), *GetNameSafe(CharacterOwner->GetMovementBase()));
//
//				const FVector RMTranslation = RootMotionParams.GetRootMotionTransform().GetTranslation();
//				const FRotator RMRotation = RootMotionParams.GetRootMotionTransform().GetRotation().Rotator();
//				UE_LOG(LogRootMotion, Warning, TEXT("PerformMovement Resulting DeltaError Translation: %s, Rotation: %s"),
//					*(ResultingLocation - OldLocation - RMTranslation).ToCompactString(), *(ResultingRotation - OldActorRotation - RMRotation).GetNormalized().ToCompactString());
//			}
//#endif // !(UE_BUILD_SHIPPING)
//
//			// Root Motion has been used, clear
//			RootMotionParams.Clear();
//		}
//		else if (CurrentRootMotion.HasActiveRootMotionSources())
//		{
//			FQuat RootMotionRotationQuat;
//			if (CharacterOwner && UpdatedComponent && CurrentRootMotion.GetOverrideRootMotionRotation(DeltaSeconds, *CharacterOwner, *this, RootMotionRotationQuat))
//			{
//				const FQuat OldActorRotationQuat = UpdatedComponent->GetComponentQuat();
//				const FQuat NewActorRotationQuat = RootMotionRotationQuat * OldActorRotationQuat;
//				MoveUpdatedComponent(FVector::ZeroVector, NewActorRotationQuat, true);
//			}
//		}
		*/

		//AI
		/*
		// consume path following requested velocity
		m_lastUpdateRequestedVelocity = bHasRequestedVelocity ? RequestedVelocity : FVector::ZeroVector;
		bHasRequestedVelocity = false;*/

		OnMovementUpdated(DeltaTime, lOldLocation, lOldVelocity);
	} // End scoped movement update

	// Call external post-movement events. These happen after the scoped movement completes in case the events want to use the current state of overlaps etc.
	CallMovementUpdateDelegate(DeltaTime, lOldLocation, lOldVelocity);

	UpdateComponentVelocity();

	//Net code
	/*
	//const bool bHasAuthority = CharacterOwner && CharacterOwner->HasAuthority();

	//// If we move we want to avoid a long delay before replication catches up to notice this change, especially if it's throttling our rate.
	//if (bHasAuthority && UNetDriver::IsAdaptiveNetUpdateFrequencyEnabled() && UpdatedComponent)
	//{
	//	UNetDriver* NetDriver = MyWorld->GetNetDriver();
	//	if (NetDriver && NetDriver->IsServer())
	//	{
	//		FNetworkObjectInfo* NetActor = NetDriver->FindNetworkObjectInfo(CharacterOwner);

	//		if (NetActor && MyWorld->GetTimeSeconds() <= NetActor->NextUpdateTime && NetDriver->IsNetworkActorUpdateFrequencyThrottled(*NetActor))
	//		{
	//			if (ShouldCancelAdaptiveReplication())
	//			{
	//				NetDriver->CancelAdaptiveReplication(*NetActor);
	//			}
	//		}
	//	}
	//}*/ 

	const FVector lNewLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
	const FQuat lNewRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;

	//Net code
	/*
	if (bHasAuthority && UpdatedComponent && !IsNetMode(NM_Client))
	{
		const bool bLocationChanged = (NewLocation != LastUpdateLocation);
		const bool bRotationChanged = (NewRotation != LastUpdateRotation);
		if (bLocationChanged || bRotationChanged)
		{
			// Update ServerLastTransformUpdateTimeStamp. This is used by Linear smoothing on clients to interpolate positions with the correct delta time,
			// so the timestamp should be based on the client's move delta (ServerAccumulatedClientTimeStamp), not the server time when receiving the RPC.
			const bool bIsRemotePlayer = (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy);
			const FNetworkPredictionData_Server_Character* ServerData = bIsRemotePlayer ? GetPredictionData_Server_Character() : nullptr;
			if (bIsRemotePlayer && ServerData && CharacterMovementCVars::NetUseClientTimestampForReplicatedTransform)
			{
				ServerLastTransformUpdateTimeStamp = float(ServerData->ServerAccumulatedClientTimeStamp);
			}
			else
			{
				ServerLastTransformUpdateTimeStamp = MyWorld->GetTimeSeconds();
			}
		}
	}
	*/

	m_lastUpdateLocation = lNewLocation;
	m_lastUpdateRotation = lNewRotation;
	m_lastUpdateVelocity = Velocity;
}

void UPMCharacterMovement::StartNewPhysics_Implementation(float DeltaTime, int32 Interation)
{
}

void UPMCharacterMovement::OnMoveTagChanged_Implementation()
{
}

bool UPMCharacterMovement::IsSameMovementMode(const FGameplayTag& NewMoveTag)
{
	return IsValid(m_currentMovement) && m_currentMovementTag == NewMoveTag;
}

bool UPMCharacterMovement::ChangeMovement(const UPMMovementMode* NewMovement)
{
	m_currentMovement = const_cast<UPMMovementMode*>(NewMovement);
	m_currentMovementTag = NewMovement->MovementTag;
	OnMoveTagChanged();

	return true;
}

void UPMCharacterMovement::ClearAccumulatedForces()
{
	m_pendingImpulseToApply = FVector::ZeroVector;
	m_pendingForceToApply = FVector::ZeroVector;
	m_pendingLaunchVelocity = FVector::ZeroVector;
}

void UPMCharacterMovement::ApplyAccumulatedForces(float DeltaSeconds)
{
	if (m_pendingImpulseToApply.Z != 0.f || m_pendingForceToApply.Z != 0.f) {
		// check to see if applied momentum is enough to overcome gravity
		if (IsMovingOnGround() && (m_pendingImpulseToApply.Z + (m_pendingForceToApply.Z * DeltaSeconds) + (GetGravityZ() * DeltaSeconds) > UE_SMALL_NUMBER)) {
			SetMovementTag(Movement_Native_Mode_Fall);
		}
	}

	Velocity += m_pendingImpulseToApply + (m_pendingForceToApply * DeltaSeconds);

	// Don't call ClearAccumulatedForces() because it could affect launch velocity
	m_pendingImpulseToApply = m_pendingForceToApply = FVector::ZeroVector;
}

bool UPMCharacterMovement::SetMovementTag(const FGameplayTag& NewMoveTag)
{
	bool lHasChangeTag = false;

	if (IsSameMovementMode(NewMoveTag)) {
		lHasChangeTag = false;
		return lHasChangeTag;
	}

	UClass* lMoveClass = m_allMoveMode.Find(NewMoveTag)->Get();

	if (IsValid(lMoveClass)) {

		UPMMovementMode* lTmp = lMoveClass->GetDefaultObject<UPMMovementMode>();

		if (lTmp == nullptr) {
			lHasChangeTag = false;
			return lHasChangeTag;
		}

		if (!ChangeMovement(lTmp)) {
			lHasChangeTag = false;
		}
		else {
			lHasChangeTag = true;
		}
	}

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

	if (IsValid(UpdatedPrimitive) && UpdatedPrimitive->IsA<UCapsuleComponent>()) {
		m_capsuleComponent = Cast<UCapsuleComponent>(UpdatedPrimitive);
	}
	else {
		if (GetPawnOwner()) {
			m_capsuleComponent = GetPawnOwner()->FindComponentByClass<UCapsuleComponent>();
		}
	}

	if (!m_capsuleComponent) {
		//DEBUG ERROR
	}

	if (bFindMeshInOwner && GetPawnOwner()) {
		m_ownerMesh = GetPawnOwner()->FindComponentByClass<USkeletalMeshComponent>();
	}

	if (!m_ownerMesh && bFindMeshInOwner) {
		//DEBUG ERROR
	}
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
