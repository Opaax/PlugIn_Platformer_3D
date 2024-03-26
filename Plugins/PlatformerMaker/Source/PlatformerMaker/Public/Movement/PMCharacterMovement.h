// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PMAcceleration.h"
#include "PMBraking.h"
#include "PMCharacterMovementPostPhysicsTickFunction.h"
#include "PMCharacterMovementPrePhysicsTickFunction.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "PMCharacterMovement.generated.h"

class UPMMovementMode;

//DELEGATES
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPMCharacterMovementUpdatedSignature,const APawn*, MovementOwningPawn, float, DeltaSeconds, FVector, OldLocation, FVector, OldVelocity);

//TAGS
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Native_Mode_Walking);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Native_Mode_Crouch);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Native_Mode_Fall);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Native_Mode_Jump);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Native_Sate_WantsCrouch);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Native_Sate_IsCrouch);

/**
 * Custom Character movement component for Platformer Maker
 * 
 * Not attended to remplace Unreal Component, more likely readable component.
 * Use command pattern externalize movement phys algo in oher files.
 * 
 * Do not contains
 *	- Root motion
 *	- Net code
 */
UCLASS()
class PLATFORMERMAKER_API UPMCharacterMovement : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
protected:
	UPROPERTY(EditAnywhere, Category = "Movement|Acceleration", BlueprintReadOnly, meta = (DisplayName = "Acceleration"))
	FPMAcceleration m_acceleration;

	UPROPERTY(EditAnywhere, Category = "Movement|Modes", BlueprintReadOnly, meta = (DisplayName = "AllMoveMode"))
	TMap<FGameplayTag, TSoftClassPtr<UPMMovementMode>> m_allMoveMode;

	UPROPERTY(VisibleAnywhere, Category = "Movement", BlueprintReadOnly, meta = (DisplayName = "CurrentMovementTag"))
	FGameplayTag m_currentMovementTag;

	UPROPERTY(VisibleAnywhere, Category = "Movement", BlueprintReadOnly, meta = (DisplayName = "PreviousMovementTag"))
	FGameplayTag m_previousMovementTag;

	UPROPERTY(VisibleAnywhere, Category = "Movement", BlueprintReadWrite, meta = (DisplayName = "CurrentMovement"))
	TObjectPtr<UPMMovementMode> m_currentMovement;

	UPROPERTY(VisibleAnywhere, Category = "Runtime", BlueprintGetter = "GetCapsuleComponent", meta = (AllowPrivateAccess = "True"))
	TObjectPtr<UCapsuleComponent> m_capsuleComponent;

	/* all tags to consider IsWalkingOnGround */
	UPROPERTY(Category = "General_Settings", EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer m_tagsMeaningWalks;

	/* all tags to validate CanStateCrouch */
	UPROPERTY(Category = "General_Settings", EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer m_allTagsToAllowCrouch;

	UPROPERTY(VisibleAnywhere, Category = "Runtime")
	FVector m_inputVector;

	float m_owningDeltaTime = 0;
	int32 m_owningInteration = 0;
#pragma region CharacterMovementUE Interface
	/**
	 * If true, high-level movement updates will be wrapped in a movement scope that accumulates updates and defers a bulk of the work until the end.
	 * When enabled, touch and hit events will not be triggered until the end of multiple moves within an update, which can improve performance.
	 *
	 * @see FScopedMovementUpdate
	 */
	UPROPERTY(Category = "Movement|General_Settings", EditAnywhere, AdvancedDisplay)
	uint8 bEnableScopedMovementUpdates : 1;

	/**
	 * Whether the character ignores changes in rotation of the base it is standing on.
	 * If true, the character maintains current world rotation.
	 * If false, the character rotates with the moving base.
	 */
	UPROPERTY(Category = "Movement|General_Settings", EditAnywhere, BlueprintReadWrite)
	uint8 bIgnoreBaseRotation : 1;

	/**
	 * If true, smoothly rotate the Character toward the Controller's desired rotation (typically Controller->ControlRotation), using RotationRate as the rate of rotation change. Overridden by OrientRotationToMovement.
	 * Normally you will want to make sure that other settings are cleared, such as bUseControllerRotationYaw on the Character.
	 */
	UPROPERTY(Category = "Movement|General_Settings", EditAnywhere, BlueprintReadWrite)
	uint8 bUseControllerDesiredRotation : 1;

	/**
	 * If true, rotate the Character toward the direction of acceleration, using RotationRate as the rate of rotation change. Overrides UseControllerDesiredRotation.
	 * Normally you will want to make sure that other settings are cleared, such as bUseControllerRotationYaw on the Character.
	 */
	UPROPERTY(Category = "Movement|General_Settings", EditAnywhere, BlueprintReadWrite)
	uint8 bOrientRotationToMovement : 1;

	/**
	 * Set this to true if riding on a moving base that you know is clear from non-moving world obstructions.
	 * Optimization to avoid sweeps during based movement, use with care.
	 */
	UPROPERTY()
	uint8 bFastAttachedMove : 1;

	/** True if the UpdatedComponent was moved outside of this CharacterMovementComponent since the last move -- its starting location for this update doesn't match its ending position for the previous update. */
	UPROPERTY(Category = "Runtime", VisibleInstanceOnly, AdvancedDisplay)
	bool bTeleportedSinceLastUpdate = false;

	/**
	 * Force the Character in Valid movement to do a check for a valid floor even if it hasn't moved. Cleared after next floor check.
	 * Normally if bAlwaysCheckFloor is false we try to avoid the floor check unless some conditions are met, but this can be used to force the next check to always run.
	 */
	UPROPERTY(Category = "Runtime", VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay)
	uint8 bForceNextFloorCheck : 1;

	/** Used by movement code to determine if a change in position is based on normal movement or a teleport. If not a teleport, velocity can be recomputed based on the change in position. */
	UPROPERTY(Category = "Runtime", VisibleInstanceOnly, BlueprintReadWrite, BlueprintGetter = "IsJustTeleported", BlueprintSetter = "SetJustTeleported")
	uint8 bJustTeleported : 1;

	/**
	 * Location after last PerformMovement or SimulateMovement update. Used internally to detect changes in position from outside character movement to try to validate the current floor.
	 */
	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	FVector m_lastUpdateLocation;

	/**
	* Rotation after last PerformMovement or SimulateMovement update.
	*/
	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	FQuat m_lastUpdateRotation;

	/**
	* Velocity after last PerformMovement update. Used internally to detect changes in velocity from external sources.
	*/
	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	FVector m_lastUpdateVelocity;

	/** Accumulated impulse to be added next tick. */
	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	FVector m_pendingImpulseToApply;

	/** Accumulated force to be added next tick. */
	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	FVector m_pendingForceToApply;

	/** Temporarily holds launch velocity when pawn is to be launched so it happens at end of movement. */
	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	FVector m_pendingLaunchVelocity;

	/** Flag set in pre-physics update to indicate that based movement should be updated post-physics */
	uint8 bDeferUpdateBasedMovement : 1;

	/** Saved location of object we are standing on, for UpdateBasedMovement() to determine if base moved in the last frame, and therefore pawn needs an update. */
	FQuat m_oldBaseQuat;
	/** Saved location of object we are standing on, for UpdateBasedMovement() to determine if base moved in the last frame, and therefore pawn needs an update. */
	FVector m_oldBaseLocation;

	/**
	 * Max number of iterations used for each discrete simulation step.
	 * Used primarily in the the more advanced movement modes that break up larger time steps (usually those applying gravity such as falling and walking).
	 * Increasing this value can address issues with fast-moving objects or complex collision scenarios, at the cost of performance.
	 *
	 * WARNING: if (MaxSimulationTimeStep * MaxSimulationIterations) is too low for the min framerate, the last simulation step may exceed MaxSimulationTimeStep to complete the simulation.
	 * @see MaxSimulationTimeStep
	 */
	UPROPERTY(Category = "Movement|General_Settings", BlueprintGetter = "GetMaxSimInterations" EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "1", ClampMax = "25", UIMin = "1", UIMax = "25"))
	int32 m_maxSimulationIterations;

	/**
	 * Max time delta for each discrete simulation step.
	 * Used primarily in the the more advanced movement modes that break up larger time steps (usually those applying gravity such as falling and walking).
	 * Lowering this value can address issues with fast-moving objects or complex collision scenarios, at the cost of performance.
	 *
	 * WARNING: if (MaxSimulationTimeStep * MaxSimulationIterations) is too low for the min framerate, the last simulation step may exceed MaxSimulationTimeStep to complete the simulation.
	 * @see MaxSimulationIterations
	 */
	UPROPERTY(Category = "Movement|General_Settings", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "0.0166", ClampMax = "0.50", UIMin = "0.0166", UIMax = "0.50"))
	float m_maxSimulationTimeStep;

	/**
	 * True during movement update.
	 * Used internally so that attempts to change CharacterOwner and UpdatedComponent are deferred until after an update.
	 * @see IsMovementInProgress()
	 */
	UPROPERTY()
	uint8 bMovementInProgress : 1;
#pragma endregion CharacterMovementUE Interface

#pragma region Mesh
	/* let make it true for applying force on ragdoll on character*/
	UPROPERTY(Category = "General_Settings", EditAnywhere, BlueprintReadWrite)
	uint8 bFindMeshInOwner : 1;

	UPROPERTY(Category = "Runtime", VisibleInstanceOnly)
	bool bHasMeshInOwner = false;

	UPROPERTY(Category = "Runtime", VisibleInstanceOnly)
	TObjectPtr<USkeletalMeshComponent> m_ownerMesh;
#pragma endregion Mesh

#pragma region Delegates
	/**
	 * Event triggered at the end of a CharacterMovementComponent movement update.
	 * This is the preferred event to use rather than the Tick event when performing custom updates to CharacterMovement properties based on the current state.
	 * which allows this event to update as well, while a Tick event would not.
	 *
	 * @param	MovementOwningPawn	Which pawn own this movement
	 * @param	DeltaSeconds		Delta time in seconds for this update
	 * @param	InitialLocation		Location at the start of the update. May be different than the current location if movement occurred.
	 * @param	InitialVelocity		Velocity at the start of the update. May be different than the current velocity.
	 */
	UPROPERTY(BlueprintAssignable, Category = Character)
	FPMCharacterMovementUpdatedSignature OnMovementUpdatedDelegate;
#pragma endregion Delegates

	/*---------------------------------- FUNCTION ----------------------------------*/
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "PerformMovement"))
	void PerformMovement(float DeltaTime);
	virtual void PerformMovement_Implementation(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "StartNewPhysics"))
	void StartNewPhysics(float DeltaTime, int32 Interation);
	virtual void StartNewPhysics_Implementation(float DeltaTime, int32 Interation);

	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "OnMoveTagChanged"))
	void OnMoveTagChanged();
	virtual void OnMoveTagChanged_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsSameMovementMode(const FGameplayTag& NewMoveTag);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool ChangeMovement(const UPMMovementMode* NewMovement);

	UFUNCTION()
	void FindCapsuleComponent();

#pragma region CharacterMovementUE Interface
protected:
	UFUNCTION(BlueprintCallable, Category = "Movement|Forces")
	void ClearAccumulatedForces();

	/** Applies momentum accumulated through AddImpulse() and AddForce(), then clears those forces. Does *not* use ClearAccumulatedForces() since that would clear pending launch velocity as well. */
	UFUNCTION(BlueprintCallable, Category = "Movement|Forces")
	virtual void ApplyAccumulatedForces(float DeltaSeconds);

	/**
	 * Event triggered when we are moving on a base but we are not able to move the full DeltaPosition because something has blocked us.
	 * Note: MoveComponentFlags includes the flag to ignore the movement base while this event is fired.
	 * @param DeltaPosition		How far we tried to move with the base.
	 * @param OldLocation		Location before we tried to move with the base.
	 * @param MoveOnBaseHit		Hit result for the object we hit when trying to move with the base.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "OnUnableToFollowBaseMove"))
	void OnUnableToFollowBaseMove(const FVector& DeltaPosition, const FVector& OldLocation, const FHitResult& MoveOnBaseHit);
	virtual void OnUnableToFollowBaseMove_Implementation(const FVector& DeltaPosition, const FVector& OldLocation, const FHitResult& MoveOnBaseHit);

	/**
	 * Event triggered at the end of a movement update. If scoped movement updates are enabled (bEnableScopedMovementUpdates), this is within such a scope.
	 * If that is not desired, bind to the CharacterOwner's OnMovementUpdated event instead, as that is triggered after the scoped movement update.
	 */
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity);

	/** Internal function to call OnMovementUpdated delegate on CharacterOwner. */
	virtual void CallMovementUpdateDelegate(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity);

public:
	/** Tick function called after physics (sync scene) has finished simulation, before cloth */
	virtual void PostPhysicsTickComponent(float DeltaTime, FPMCharacterMovementPostPhysicsTickFunction& ThisTickFunction);

	/** Tick function called before physics */
	virtual void PrePhysicsTickComponent(float DeltaTime, FPMCharacterMovementPrePhysicsTickFunction& ThisTickFunction);

	/** Post-physics tick function for this character */
	UPROPERTY()
	struct FPMCharacterMovementPostPhysicsTickFunction PostPhysicsTickFunction;

	/** Pre-physics tick function for this character */
	UPROPERTY()
	struct FPMCharacterMovementPrePhysicsTickFunction PrePhysicsTickFunction;

	/**
	 * Compute remaining time step given remaining time and current iterations.
	 * The last iteration (limited by MaxSimulationIterations) always returns the remaining time, which may violate MaxSimulationTimeStep.
	 *
	 * @param RemainingTime		Remaining time in the tick.
	 * @param Iterations		Current iteration of the tick (starting at 1).
	 * @return The remaining time step to use for the next sub-step of iteration.
	 * @see MaxSimulationTimeStep, MaxSimulationIterations
	 */
	float GetSimulationTimeStep(float RemainingTime, int32 Iterations) const;

	/** Update position based on Base movement */
	virtual void UpdateBasedMovement(float DeltaSeconds);

	/** Update or defer updating of position based on Base movement */
	virtual void MaybeUpdateBasedMovement(float DeltaSeconds);

	/** Update controller's view rotation as pawn's base rotates */
	virtual void UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation);

	/** Update the character state in PerformMovement right before doing the actual position change */
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds);
	/** Update the character state in PerformMovement right after doing the actual position change */
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds);

	/** Handle a pending launch during an update. Returns true if the launch was triggered. */
	virtual bool HandlePendingLaunch();

	/** Updates acceleration and perform movement, called from the TickComponent on the authoritative side for controlled characters,
	 *	or on the client for characters without a controller when either playing root motion or bRunPhysicsWithNoController is true.
	 */
	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds);

	/** Scale input acceleration, based on movement acceleration rate. */
	virtual FVector ScaleInputAcceleration(const FVector& InputAcceleration) const;

	/** Enforce constraints on input given current state. For instance, don't move upwards if walking and looking up. */
	virtual FVector ConstrainInputAcceleration(const FVector& InputAcceleration) const;

#pragma endregion CharacterMovementUE Interface

public:
	UPMCharacterMovement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*/!\ VERY IMPORTANT /!\*\

	/*
	* The UpdateComponent could be clear by the Super::TickComponent in MovementComponentBase class
	* It could be clear like UpdateComponent is not null but no valid because the update component is pending kill but not kill yet
	*/
	UFUNCTION(Category = "Movement")
	bool IsValidToMove();

	UFUNCTION(Category = "Movement")
	bool IsValidBaseData();

	UFUNCTION(BlueprintPure, Category = "Movement|Acceleration")
	FORCEINLINE FPMAcceleration GetAcceleration() const { return m_acceleration; }

	UFUNCTION(BlueprintCallable, Category = "Movement|Acceleration")
	void GetAccelerationRef(FPMAcceleration& OutAcceleration) const { OutAcceleration = m_acceleration; }

	UFUNCTION(BlueprintPure, Category = "Movement|Acceleration")
	FORCEINLINE FVector GetAccelerationVector() const { return m_acceleration.GetAcceleration(); }

	UFUNCTION(BlueprintCallable, Category = "Movement|Acceleration")
	void GetAccelerationVectorRef(FVector& OutAccelerationVector) const { OutAccelerationVector = m_acceleration.GetAcceleration(); }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool SetMovementTag(const FGameplayTag& NewMoveTag);

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE UCapsuleComponent* GetCapsuleComponent() const { return m_capsuleComponent; }

	UFUNCTION(BlueprintCallable)
	void SetVelocity(FVector InVel);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement|GetterSetter")
	FORCEINLINE bool IsJustTeleported() const { return (bool)bJustTeleported; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement|GetterSetter")
	FORCEINLINE void SetJustTeleported(bool InValue) { bJustTeleported = InValue; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE int32 GetMaxSimInterations() const { return m_maxSimulationIterations; }

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	//UObject
	virtual void PostLoad() override;

	//UActorComp
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void BeginPlay() override;

	//Movement Comp
	virtual float GetGravityZ() const override;
	virtual bool IsMovingOnGround() const override;
	virtual bool IsFalling() const override;
	virtual float GetMaxSpeed() const override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;

	/*---------------------------------- CONST ----------------------------------*/
public:
	/** Minimum delta time considered when ticking. Delta times below this are not considered. This is a very small non-zero value to avoid potential divide-by-zero in simulation code. */
	static const float MIN_TICK_TIME;

	/** Minimum acceptable distance for Character capsule to float above floor when walking. */
	static const float MIN_FLOOR_DIST;

	/** Maximum acceptable distance for Character capsule to float above floor when walking. */
	static const float MAX_FLOOR_DIST;

	/** Reject sweep impacts that are this close to the edge of the vertical portion of the capsule when performing vertical sweeps, and try again with a smaller capsule. */
	static const float SWEEP_EDGE_REJECT_DISTANCE;

	/** Stop completely when braking and velocity magnitude is lower than this. */
	static const float BRAKE_TO_STOP_VELOCITY;
};
