// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Movement/PMCharacterMovement.h"
#include "Movement/PMMovementMode.h"
#include "Utils/DebugMacro.h"

//Unreal
#include "Engine/ScopedMovementUpdate.h"
#include "GameFramework/Character.h" //Need it for namespace utility
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PMCharacterMovement)

//TAGS
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Mode_Walking, "Movement.Native.Mode.Walking");
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Mode_Crouch, "Movement.Native.Mode.Crouch");
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Mode_Fall, "Movement.Native.Mode.Fall");
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Mode_Jump, "Movement.Native.Mode.Jump");

UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Sate_WantsCrouch, "Movement.Native.Sate.WantsCrouch");
UE_DEFINE_GAMEPLAY_TAG(Movement_Native_Sate_IsCrouch, "Movement.Native.Sate.IsCrouch");

//Const
const float UPMCharacterMovement::MIN_TICK_TIME = 1e-6f;
const float UPMCharacterMovement::MIN_FLOOR_DIST = 1.9f;
const float UPMCharacterMovement::MAX_FLOOR_DIST = 2.4f;
const float UPMCharacterMovement::BRAKE_TO_STOP_VELOCITY = 10.f;
const float UPMCharacterMovement::SWEEP_EDGE_REJECT_DISTANCE = 0.15f;

DEFINE_LOG_CATEGORY_STATIC(LogPMCharacterMovement, Log, All);

UPMCharacterMovement::UPMCharacterMovement(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	m_maxSimulationIterations = 8;
}

void UPMCharacterMovement::ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds)
{
	{
		//// We need to check the jump state before adjusting input acceleration, to minimize latency
		//// and to make sure acceleration respects our potentially new falling state.
		//CharacterOwner->CheckJumpInput(DeltaSeconds);

		// apply input to acceleration
		m_acceleration.SetAcceleration(ScaleInputAcceleration(ConstrainInputAcceleration(InputVector)));
		//AnalogInputModifier = ComputeAnalogInputModifier();
	}

	PerformMovement(DeltaSeconds);
}

FVector UPMCharacterMovement::ScaleInputAcceleration(const FVector& InputAcceleration) const
{
	return m_acceleration.MaxAcceleration * InputAcceleration.GetClampedToMaxSize(1.0f);
}

FVector UPMCharacterMovement::ConstrainInputAcceleration(const FVector& InputAcceleration) const
{
	// walking or falling pawns ignore up/down sliding
	if (InputAcceleration.Z != 0.f && (IsMovingOnGround() || IsFalling())) {
		return FVector(InputAcceleration.X, InputAcceleration.Y, 0.f);
	}

	return InputAcceleration;
}

bool UPMCharacterMovement::IsValidToMove()
{
	return IsValid(GetController()) && IsValidBaseData() && IsValid(m_currentMovement) && m_currentMovementTag.IsValid();
}

bool UPMCharacterMovement::IsValidBaseData() const
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

float UPMCharacterMovement::GetSimulationTimeStep(float RemainingTime, int32 Iterations) const
{
	static uint32 s_WarningCount = 0;
	if (RemainingTime > m_maxSimulationTimeStep)
	{
		if (Iterations < m_maxSimulationIterations)
		{
			// Subdivide moves to be no longer than MaxSimulationTimeStep seconds
			RemainingTime = FMath::Min(m_maxSimulationTimeStep, RemainingTime * 0.5f);
		}
		else
		{
			// If this is the last iteration, just use all the remaining time. This is usually better than cutting things short, as the simulation won't move far enough otherwise.
			// Print a throttled warning.
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if ((s_WarningCount++ < 100) || (GFrameCounter & 15) == 0)
			{
				UE_LOG(LogPMCharacterMovement, Warning, TEXT("GetSimulationTimeStep() - Max iterations %d hit while remaining time %.6f > MaxSimulationTimeStep (%.3f) for '%s', movement '%s'"), m_maxSimulationIterations, RemainingTime, m_maxSimulationTimeStep, *GetNameSafe(GetPawnOwner()), *m_currentMovementTag.ToString());
			}
#endif
		}
	}

	// no less than MIN_TICK_TIME (to avoid potential divide-by-zero during simulation).
	return FMath::Max(MIN_TICK_TIME, RemainingTime);
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
	if (!m_currentMovementTag.IsValid() || UpdatedComponent->Mobility != EComponentMobility::Movable || UpdatedComponent->IsSimulatingPhysics()) {
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

		if (m_previousMovementTag != m_currentMovementTag) {
			if (!m_currentMovement->StartMovement(DeltaTime, this)) {
				SetMovementTag(Movement_Native_Mode_Fall);
			}
		}
		// change position
		m_owningDeltaTime = DeltaTime;
		m_owningInteration = 0;
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
	if ((DeltaTime < MIN_TICK_TIME) || (Interation >= m_maxSimulationIterations) || !IsValidBaseData()) {
		return;
	}

	if (UpdatedComponent->IsSimulatingPhysics()) {
		return;
	}

	DEBUG_LOG_SCREEN(1, 5.F, FColor::Red, TEXT("Velocity: %s"), *Velocity.ToString());
	DEBUG_LOG_SCREEN(2, 5.F, FColor::Blue, TEXT("Acceleration: %s"), *m_acceleration.GetAcceleration().ToString());
	DEBUG_LOG_SCREEN(3, 5.F, FColor::Green, TEXT("Input: %s"), *m_inputVector.ToString());

	const bool bSavedMovementInProgress = bMovementInProgress;
	bMovementInProgress = true;

	if (IsValid(m_currentMovement)) {
		m_currentMovement->PhysMovement(DeltaTime, Interation, this);
	} else {
		SetMovementTag(Movement_Native_Mode_Walking);
	}

	bMovementInProgress = bSavedMovementInProgress;
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
	m_previousMovementTag = m_currentMovementTag;
	m_currentMovement = const_cast<UPMMovementMode*>(NewMovement);
	m_currentMovementTag = NewMovement->MovementTag;

	OnMoveTagChanged();

	DEBUG_LOG_SCREEN(-1, 10, FColor::Red, TEXT("%s change movement %s"), *GetNameSafe(this), *GetNameSafe(NewMovement));

	return 	m_currentMovement->StartMovement(m_owningDeltaTime, this);
}

void UPMCharacterMovement::FindCapsuleComponent()
{
	if (IsValid(UpdatedPrimitive) && UpdatedPrimitive->IsA<UCapsuleComponent>()) {
		m_capsuleComponent = Cast<UCapsuleComponent>(UpdatedPrimitive);
	}
	else {
		if (GetPawnOwner()) {
			m_capsuleComponent = GetPawnOwner()->FindComponentByClass<UCapsuleComponent>();
		}
	}

	if (!m_capsuleComponent) {
		DEBUG_ERROR(TEXT("[%s] no capsule comp"), *GetNameSafe(this));
		//DEBUG ERROR
	}
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
		DEBUG_LOG_SCREEN(-1, 10, FColor::Red, TEXT("%s Same on new tag set "), *GetNameSafe(this));
		lHasChangeTag = false;
		return lHasChangeTag;
	}

	// I need to optimize that
	if (!m_allMoveMode.Find(NewMoveTag)) {
		DEBUG_LOG_SCREEN(-1, 10, FColor::Red, TEXT("%s change movement tag not found"), *GetNameSafe(this));
		return false;
	}

	UClass* lMoveClass = const_cast<UClass*>(m_allMoveMode.Find(NewMoveTag)->Get());

	if (!lMoveClass) {
		m_allMoveMode.Find(NewMoveTag)->LoadSynchronous();
		lMoveClass = const_cast<UClass*>(m_allMoveMode.Find(NewMoveTag)->Get());
	}
	//

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
	else {
		DEBUG_LOG_SCREEN(-1, 10, FColor::Red, TEXT("%s move class not valid"), *GetNameSafe(this));
	}

	return lHasChangeTag;
}

void UPMCharacterMovement::SetVelocity(FVector InVel)
{
	Velocity = InVel;
}

/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
/*------------------------------------- OVERRIDES -------------------------------------*/
/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

void UPMCharacterMovement::PostLoad()
{
	Super::PostLoad();

	FindCapsuleComponent();

	if (bFindMeshInOwner && GetPawnOwner()) {
		m_ownerMesh = GetPawnOwner()->FindComponentByClass<USkeletalMeshComponent>();
	}

	if (!m_ownerMesh && bFindMeshInOwner) {
		//DEBUG ERROR
	}
}

void UPMCharacterMovement::BeginPlay()
{
	Super::BeginPlay();

	FindCapsuleComponent();
}

float UPMCharacterMovement::GetGravityZ() const
{
	return Super::GetGravityZ();
}

bool UPMCharacterMovement::IsMovingOnGround() const
{
	return m_currentMovementTag == Movement_Native_Mode_Walking && IsValid(UpdatedComponent);
}

bool UPMCharacterMovement::IsFalling() const
{
	return m_currentMovementTag == Movement_Native_Mode_Fall && UpdatedComponent;
}

float UPMCharacterMovement::GetMaxSpeed() const
{
	return IsValid(m_currentMovement) ? m_currentMovement->GetMaxSpeed() : 0;
}

void UPMCharacterMovement::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);
}

void UPMCharacterMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!IsValidBaseData() || ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	m_inputVector = ConsumeInputVector();

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Super tick may destroy/invalidate CharacterOwner or UpdatedComponent, so we need to re-check.
	if (!IsValidBaseData()) {
		return;
	}

	// See if we fell out of the world.
	const bool lbIsSimulatingPhysics = UpdatedComponent->IsSimulatingPhysics();

	if ((lbIsSimulatingPhysics) && !GetPawnOwner()->CheckStillInWorld()) {
		return;
	}

	// We don't update if simulating physics (eg ragdolls).
	if (lbIsSimulatingPhysics) {
		ClearAccumulatedForces();
		return;
	}

	if (GetPawnOwner()->IsLocallyControlled() || !GetController()) {
		ControlledCharacterMove(m_inputVector, DeltaTime);
	}
}

void UPMCharacterMovement::CalcVelocity_Implementation(float DeltaTime, FPMBraking BrakingSetting, bool bFluid)
{
	// Do not update velocity
	if (!IsValidBaseData() || DeltaTime < MIN_TICK_TIME) {
		return;
	}

	float lFriction = FMath::Max(0.f, BrakingSetting.BrakingFriction);
	const float lMaxAccel = m_acceleration.MaxAcceleration;
	float lMaxSpeed = GetMaxSpeed();


	// Apply braking or deceleration
	const bool bZeroAcceleration = m_acceleration.GetAcceleration().IsZero();
	const bool bVelocityOverMax = IsExceedingMaxSpeed(lMaxSpeed);

	// Only apply braking if there is no acceleration, or we are over our max speed and need to slow down to it.
	if ((bZeroAcceleration) || bVelocityOverMax) {
		const FVector lOldVelocity = Velocity;

		const float lActualBrakingFriction = lFriction;
		ApplyVelocityBraking(DeltaTime, BrakingSetting, lActualBrakingFriction);

		// Don't allow braking to lower us below max speed if we started above it.
		if (bVelocityOverMax && Velocity.SizeSquared() < FMath::Square(lMaxSpeed) && FVector::DotProduct(m_acceleration.GetAcceleration(), lOldVelocity) > 0.0f) {
			Velocity = lOldVelocity.GetSafeNormal() * lMaxSpeed;
		}
	}
	else if (!bZeroAcceleration) {
		// Friction affects our ability to change direction. This is only done for input acceleration, not path following.
		const FVector lAccelDir = m_acceleration.GetAcceleration().GetSafeNormal();
		const float lVelSize = Velocity.Size();
		Velocity = Velocity - (Velocity - lAccelDir * lVelSize) * FMath::Min(DeltaTime * lFriction, 1.f);
	}

	// Apply fluid friction
	if (bFluid) {
		Velocity = Velocity * (1.f - FMath::Min(lFriction * DeltaTime, 1.f));
	}

	// Apply input acceleration
	if (!bZeroAcceleration) {
		const float lNewMaxInputSpeed = IsExceedingMaxSpeed(lMaxSpeed) ? Velocity.Size() : lMaxSpeed;
		Velocity += m_acceleration.GetAcceleration() * DeltaTime;
		Velocity = Velocity.GetClampedToMaxSize(lNewMaxInputSpeed);
	}
}

void UPMCharacterMovement::ApplyVelocityBraking_Implementation(float DeltaTime, FPMBraking BrakingSetting, float ComputedFriction)
{
	if (Velocity.IsZero() || !IsValidBaseData() || DeltaTime < MIN_TICK_TIME) {
		return;
	}

	const float lFrictionFactor = FMath::Max(0.f, BrakingSetting.BrakingFrictionFactor);
	ComputedFriction = FMath::Max(0.f, ComputedFriction * lFrictionFactor);
	float lBrakingDeceleration = FMath::Max(0.f, BrakingSetting.BrakingDeceleration);
	const bool bZeroFriction = (ComputedFriction == 0.f);
	const bool bZeroBraking = (lBrakingDeceleration == 0.f);

	if (bZeroFriction && bZeroBraking) {
		return;
	}

	const FVector lOldVel = Velocity;

	// subdivide braking to get reasonably consistent results at lower frame rates
	// (important for packet loss situations w/ networking)
	float lRemainingTime = DeltaTime;
	const float lMaxTimeStep = FMath::Clamp(m_brakingSubStepTime, 1.0f / 75.0f, 1.0f / 20.0f);

	// Decelerate to brake to a stop
	const FVector lRevAccel = (bZeroBraking ? FVector::ZeroVector : (-lBrakingDeceleration * Velocity.GetSafeNormal()));
	while (lRemainingTime >= MIN_TICK_TIME) {
		// Zero friction uses constant deceleration, so no need for iteration.
		const float ldt = ((lRemainingTime > lMaxTimeStep && !bZeroFriction) ? FMath::Min(lMaxTimeStep, lRemainingTime * 0.5f) : lRemainingTime);
		lRemainingTime -= ldt;

		// apply friction and braking
		Velocity = Velocity + ((-ComputedFriction) * Velocity + lRevAccel) * ldt;

		// Don't reverse direction
		if ((Velocity | lOldVel) <= 0.f) {
			Velocity = FVector::ZeroVector;
			return;
		}
	}

	// Clamp to zero if nearly zero, or if below min threshold and braking.
	const float VSizeSq = Velocity.SizeSquared();
	if (VSizeSq <= UE_KINDA_SMALL_NUMBER || (!bZeroBraking && VSizeSq <= FMath::Square(BRAKE_TO_STOP_VELOCITY))) {
		Velocity = FVector::ZeroVector;
	}
}

void UPMCharacterMovement::OnCharacterStuckInGeometry(const FHitResult* Hit)
{
	bJustTeleported = true;
}

void UPMCharacterMovement::FindFloor(const FVector& CapsuleLocation, FPMFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult) const
{
	// No collision, no floor...
	if (!IsValidBaseData() || !UpdatedComponent->IsQueryCollisionEnabled()) {
		OutFloorResult.Clear();
		return;
	}

	// Increase height check slightly if walking, to prevent floor height adjustment from later invalidating the floor result.
	const float HeightCheckAdjust = (IsMovingOnGround() ? MAX_FLOOR_DIST + UE_KINDA_SMALL_NUMBER : -MAX_FLOOR_DIST);

	float FloorSweepTraceDist = FMath::Max(MAX_FLOOR_DIST, MaxStepHeight + HeightCheckAdjust);
	float FloorLineTraceDist = FloorSweepTraceDist;
	bool bNeedToValidateFloor = true;

	// Sweep floor
	if (FloorLineTraceDist > 0.f || FloorSweepTraceDist > 0.f)
	{
		UCharacterMovementComponent* MutableThis = const_cast<UCharacterMovementComponent*>(this);

		if (bAlwaysCheckFloor || !bCanUseCachedLocation || bForceNextFloorCheck || bJustTeleported)
		{
			MutableThis->bForceNextFloorCheck = false;
			ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
		}
		else
		{
			// Force floor check if base has collision disabled or if it does not block us.
			UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
			const AActor* BaseActor = MovementBase ? MovementBase->GetOwner() : NULL;
			const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

			if (MovementBase != NULL)
			{
				MutableThis->bForceNextFloorCheck = !MovementBase->IsQueryCollisionEnabled()
					|| MovementBase->GetCollisionResponseToChannel(CollisionChannel) != ECR_Block
					|| MovementBaseUtility::IsDynamicBase(MovementBase);
			}

			const bool IsActorBasePendingKill = BaseActor && !IsValid(BaseActor);

			if (!bForceNextFloorCheck && !IsActorBasePendingKill && MovementBase)
			{
				//UE_LOG(LogCharacterMovement, Log, TEXT("%s SKIP check for floor"), *CharacterOwner->GetName());
				OutFloorResult = CurrentFloor;
				bNeedToValidateFloor = false;
			}
			else
			{
				MutableThis->bForceNextFloorCheck = false;
				ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
			}
		}
	}

	// OutFloorResult.HitResult is now the result of the vertical floor check.
	// See if we should try to "perch" at this location.
	if (bNeedToValidateFloor && OutFloorResult.bBlockingHit && !OutFloorResult.bLineTrace)
	{
		const bool bCheckRadius = true;
		if (ShouldComputePerchResult(OutFloorResult.HitResult, bCheckRadius))
		{
			float MaxPerchFloorDist = FMath::Max(MAX_FLOOR_DIST, MaxStepHeight + HeightCheckAdjust);
			if (IsMovingOnGround())
			{
				MaxPerchFloorDist += FMath::Max(0.f, PerchAdditionalHeight);
			}

			FFindFloorResult PerchFloorResult;
			if (ComputePerchResult(GetValidPerchRadius(), OutFloorResult.HitResult, MaxPerchFloorDist, PerchFloorResult))
			{
				// Don't allow the floor distance adjustment to push us up too high, or we will move beyond the perch distance and fall next time.
				const float AvgFloorDist = (MIN_FLOOR_DIST + MAX_FLOOR_DIST) * 0.5f;
				const float MoveUpDist = (AvgFloorDist - OutFloorResult.FloorDist);
				if (MoveUpDist + PerchFloorResult.FloorDist >= MaxPerchFloorDist)
				{
					OutFloorResult.FloorDist = AvgFloorDist;
				}

				// If the regular capsule is on an unwalkable surface but the perched one would allow us to stand, override the normal to be one that is walkable.
				if (!OutFloorResult.bWalkableFloor)
				{
					// Floor distances are used as the distance of the regular capsule to the point of collision, to make sure AdjustFloorHeight() behaves correctly.
					OutFloorResult.SetFromLineTrace(PerchFloorResult.HitResult, OutFloorResult.FloorDist, FMath::Max(OutFloorResult.FloorDist, MIN_FLOOR_DIST), true);
				}
			}
			else
			{
				// We had no floor (or an invalid one because it was unwalkable), and couldn't perch here, so invalidate floor (which will cause us to start falling).
				OutFloorResult.bWalkableFloor = false;
			}
		}
	}
}