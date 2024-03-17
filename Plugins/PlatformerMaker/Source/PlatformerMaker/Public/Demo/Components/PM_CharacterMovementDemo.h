// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h" //ofc Unreal put every struct in one file which that force weird include like that, as I made here
#include "PM_CharacterMovementDemo.generated.h"

struct FFindFloorResult;
struct FCollisionShape;
struct FCollisionQueryParams;
struct FCollisionResponseParams;

/* 
* This is not attented to be inherited
* All theses struct should be in independant file
* 
* Maybe it could be nice to make a plug in to make character comp easier to understand in code
* For now the character component is 12k line long which I thin is not good but on networking is better?
*/
USTRUCT(BlueprintType, Blueprintable)
struct FPMAccelerationDemo {
	GENERATED_USTRUCT_BODY()

private:
	FVector m_acceleration;

public://explicit public
	UPROPERTY(Category = "Acceleration", EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", UIMin = "0"))
	float MaxAcceleration;

protected:
	void UpdateAcceleration();

public:
	FPMAccelerationDemo():
		m_acceleration(FVector::ZeroVector),
		MaxAcceleration(0)
	{}

	FPMAccelerationDemo(const FVector InAcceleration, const float InMaxAcceleration) :
		m_acceleration(InAcceleration),
		MaxAcceleration(InMaxAcceleration)
	{}

	FORCEINLINE float GetMaxAcceleration() const { return MaxAcceleration; }

	FORCEINLINE FVector GetAcceleration() const { return m_acceleration; }

	void GetAccelerationRef(FVector& OutAcceleration) const { OutAcceleration = m_acceleration; }

	void SetAccelerationRef(FVector NewAcceleration) { m_acceleration = NewAcceleration; UpdateAcceleration(); }

	void AccumulateAccelerationRef(FVector NewAcceleration) { m_acceleration += NewAcceleration; UpdateAcceleration(); }

	FORCEINLINE void SetMaxAcceleration(float NewMaxAcceleration) { MaxAcceleration = NewMaxAcceleration; }
};

USTRUCT(BlueprintType, Blueprintable)
struct FPMBrakingDemo {
	GENERATED_USTRUCT_BODY()

private:
	float m_brakingFriction;
	float m_brakingDecelerationWalking;
	float m_brakingFrictionFactor;

protected:

public:
	FPMBrakingDemo() :
		m_brakingFriction(1),
		m_brakingDecelerationWalking(500.f),
		m_brakingFrictionFactor(1)
	{}

	FPMBrakingDemo(const float InBrakingFriction, const float InBrakingDecelerationWalking, const float InBrakingFrictionFactor) :
		m_brakingFriction(InBrakingFriction),
		m_brakingDecelerationWalking(InBrakingDecelerationWalking),
		m_brakingFrictionFactor(InBrakingFrictionFactor)
	{}

	FORCEINLINE float GetBreakingFriction() const { return m_brakingFriction; }
	FORCEINLINE float GetBrakingDecelerationWalking() const { return m_brakingDecelerationWalking; }
	FORCEINLINE float GetBrakingFactor() const { return m_brakingFrictionFactor; }

	FORCEINLINE void SetBreakingFriction(float InBrakingFriction) { m_brakingFriction = InBrakingFriction; }
	FORCEINLINE void SetBrakingDecelerationWalking(float InBrakingDecelerationWalking) { m_brakingDecelerationWalking = InBrakingDecelerationWalking; }
	FORCEINLINE void SetBrakingFrictionFactor(float InBrakingFrictionFactor) { m_brakingFrictionFactor = InBrakingFrictionFactor; }
};

USTRUCT(BlueprintType, Blueprintable)
struct FPMGravityDemo {
	GENERATED_USTRUCT_BODY()

private:
	UPROPERTY(Category = "Gravity", EditDefaultsOnly, meta = (DisplayName = "GravityScale"))
	float m_gravityScale;
	float m_minGravityScale = 0.0001f;

public:

public:
	FPMGravityDemo() :
		m_gravityScale(1)
	{}

	FPMGravityDemo(const float InGravityScale)
	{
		m_gravityScale = FMath::Max(InGravityScale, m_minGravityScale);
	}

	FORCEINLINE float GetGravityScale() const { return m_gravityScale; }
	FORCEINLINE float SetGravityScale(const float NewScale) { m_gravityScale = NewScale; }
};

USTRUCT(BlueprintType, Blueprintable)
struct FPMCharacterMovementInfoDemo {
	GENERATED_USTRUCT_BODY()

private:
	/** Maximum height character can step up */
	UPROPERTY(Category = "Movement Info", EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm"))
	float m_maxStepHeight;

	/**
	 * Whether we always force floor checks for stationary Characters while walking.
	 * Normally floor checks are avoided if possible when not moving, but this can be used to force them if there are use-cases where they are being skipped erroneously
	 * (such as objects moving up into the character from below).
	 */
	UPROPERTY(Category = "Movement Info", EditAnywhere, AdvancedDisplay)
	uint8 bAlwaysCheckFloor : 1;

	/**
	 * Force the Character in MOVE_Walking to do a check for a valid floor even if it hasn't moved. Cleared after next floor check.
	 * Normally if bAlwaysCheckFloor is false we try to avoid the floor check unless some conditions are met, but this can be used to force the next check to always run.
	 */
	UPROPERTY(Category = "Movement Info", EditAnywhere, AdvancedDisplay)
	uint8 bForceNextFloorCheck : 1;

	/**
	 * Performs floor checks as if the character is using a shape with a flat base.
	 * This avoids the situation where characters slowly lower off the side of a ledge (as their capsule 'balances' on the edge).
	 */
	UPROPERTY(Category = "Movement Info", EditAnywhere, AdvancedDisplay)
	uint8 bUseFlatBaseForFloorChecks : 1;


public:
	FPMCharacterMovementInfoDemo() :
		m_maxStepHeight(45),
		bAlwaysCheckFloor(1),
		bForceNextFloorCheck(1),
		bUseFlatBaseForFloorChecks(1)
	{}

	FPMCharacterMovementInfoDemo(const float InMaxStepHeight, const uint8 InAlwaysCheckFloor = 1, const uint8 InForceNextFloorCheck = 1, const uint8 InUseFlatBaseForFloorChecks = 1) :
		m_maxStepHeight(InMaxStepHeight),
		bAlwaysCheckFloor(InAlwaysCheckFloor),
		bForceNextFloorCheck(InForceNextFloorCheck),
		bUseFlatBaseForFloorChecks(InUseFlatBaseForFloorChecks)
	{
	}

	FORCEINLINE float	GetMaxStepHeight()					const { return m_maxStepHeight; }
	FORCEINLINE uint8	GetAlwaysCheckFloor()				const { return bAlwaysCheckFloor; }
	FORCEINLINE bool	GetAlwaysCheckFloorBool()			const { return (bool)bAlwaysCheckFloor; }
	FORCEINLINE uint8	GetForceNextFloorCheck()			const { return bForceNextFloorCheck; }
	FORCEINLINE bool	GetForceNextFloorCheckBool()		const { return (bool)bForceNextFloorCheck; }
	FORCEINLINE uint8	GetUseFlatBaseForFloorChecks()		const { return bUseFlatBaseForFloorChecks; }
	FORCEINLINE bool	GetUseFlatBaseForFloorChecksBool()	const { return (bool)bUseFlatBaseForFloorChecks; }

	FORCEINLINE float	SetMaxStepHeight(const float NewMaxStepHeight) { m_maxStepHeight = NewMaxStepHeight; }
	FORCEINLINE float	SetForceNextFloorCheck(const uint8 NewForceNextFloorCheck) { bForceNextFloorCheck = NewForceNextFloorCheck; }
};

/**
 * Character movement for the Demo
 */
UCLASS()
class PLATFORMERMAKER_API UPM_CharacterMovementDemo : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
private:
	UPROPERTY(EditAnywhere, Category = "Acceleration", meta = (AllowPrivateAccess = "True", DisplayName = "Acceleration"))
	FPMAccelerationDemo m_acceleration;

	UPROPERTY(EditAnywhere, Category = "Character Braking", meta = (AllowPrivateAccess = "True", DisplayName = "Braking"))
	FPMBrakingDemo m_braking;

	UPROPERTY(EditAnywhere, Category = "Character Gravity", meta = (AllowPrivateAccess = "True", DisplayName = "Gravity"))
	FPMGravityDemo m_gravity;

	UPROPERTY(EditAnywhere, Category = "Character Info", meta = (AllowPrivateAccess = "True", DisplayName = "CharacterInfo"))
	FPMCharacterMovementInfoDemo m_characterInfo;

	/** Used by movement code to determine if a change in position is based on normal movement or a teleport. If not a teleport, velocity can be recomputed based on the change in position. */
	UPROPERTY(Category = "Runtime", Transient, VisibleInstanceOnly)
	uint8 bJustTeleported : 1;

	UPROPERTY(Transient, DuplicateTransient, Category = "Runtime", BlueprintGetter = "GetDemoOwner", BlueprintSetter = "SetDemoOwner")
	TObjectPtr<APM_CharacterDemo> m_demoOwner;

protected:
	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadOnly)
	TEnumAsByte<enum EMovementMode> m_movementMode;

	/** Information about the floor the Character is standing on (updated only during walking movement). */
	UPROPERTY(Category = "Character Movement: Walking", VisibleInstanceOnly, BlueprintGetter = "GetCurrentFloor")
	FFindFloorResult m_currentFloor;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
private:
	UFUNCTION()
	bool IsValidToMove();

	UFUNCTION()
	void CharacterControlledInput(const FVector& InputVector, float DeltaSeconds);

	UFUNCTION()
	FVector ContraintInputZ(FVector& InputVector);

	UFUNCTION()
	void PerformMovement(float DeltaSeconds);

	UFUNCTION()
	void OnMovementModeUpdated(EMovementMode NewMoveMode);

	UFUNCTION()
	void MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds);
	
	UFUNCTION()
	void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration);

	UFUNCTION()
	virtual void FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation) const;

	/**
	 * Compute distance to the floor from bottom sphere of capsule and store the result in OutFloorResult.
	 * This distance is the swept distance of the capsule to the first point impacted by the lower hemisphere, or distance from the bottom of the capsule in the case of a line trace.
	 * This function does not care if collision is disabled on the capsule (unlike FindFloor).
	 * @see FindFloor
	 *
	 * @param CapsuleLocation:	Location of the capsule used for the query
	 * @param LineDistance:		If non-zero, max distance to test for a simple line check from the capsule base. Used only if the sweep test fails to find a walkable floor, and only returns a valid result if the impact normal is a walkable normal.
	 * @param SweepDistance:	If non-zero, max distance to use when sweeping a capsule downwards for the test. MUST be greater than or equal to the line distance.
	 * @param OutFloorResult:	Result of the floor check. The HitResult will contain the valid sweep or line test upon success, or the result of the sweep upon failure.
	 * @param SweepRadius:		The radius to use for sweep tests. Should be <= capsule radius.
	 */
	UFUNCTION()
	virtual void ComputeFloorDist(float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult) const;

	/**
	 * Sweep against the world and return the first blocking hit.
	 * Intended for tests against the floor, because it may change the result of impacts on the lower area of the test (especially if bUseFlatBaseForFloorChecks is true).
	 *
	 * @param OutHit			First blocking hit found.
	 * @param Start				Start location of the capsule.
	 * @param End				End location of the capsule.
	 * @param TraceChannel		The 'channel' that this trace is in, used to determine which components to hit.
	 * @param CollisionShape	Capsule collision shape.
	 * @param Params			Additional parameters used for the trace.
	 * @param ResponseParam		ResponseContainer to be used for this trace.
	 * @return True if OutHit contains a blocking hit entry.
	 */
	virtual bool FloorSweepTest(FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam) const;

public:
	UPM_CharacterMovementDemo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Change movement mode.
	 *
	 * @param NewMovementMode	The new movement mode
	 * @param NewCustomMode		The new custom sub-mode, only applicable if NewMovementMode is Custom.
	 */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|Movement")
	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE uint8 GetJustTeleported() const { return bJustTeleported; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetJustTeleported(uint8 JustTeleported) { bJustTeleported = JustTeleported; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool GetJustTeleportedBool()const { return (bool)bJustTeleported; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE APM_CharacterDemo* GetDemoOwner() const { return m_demoOwner; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetDemoOwner(APM_CharacterDemo* InOwner) { m_demoOwner = InOwner; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FFindFloorResult GetCurrentFloor() const { return m_currentFloor; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void GetCurrentFloorRef(FFindFloorResult& OutFindFloor) const { OutFindFloor = m_currentFloor; }

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
