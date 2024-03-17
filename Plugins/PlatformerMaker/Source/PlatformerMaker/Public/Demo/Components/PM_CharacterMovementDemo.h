// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PM_CharacterMovementDemo.generated.h"

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

protected:
	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadOnly)
	TEnumAsByte<enum EMovementMode> m_movementMode;

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

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
