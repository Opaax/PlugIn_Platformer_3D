// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Movement/PMMovementMode.h"
#include "Movement/PMStepDownResultStruct.h"
#include "PMMovement_Walk.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API UPMMovement_Walk : public UPMMovementMode
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
protected:

	UPROPERTY(Category = "Movement Info|Braking", EditAnywhere, meta = (DisplayNaeme = "BrakingSettings"))
	FPMBraking m_brakingSettings;

	/** The maximum ground speed when walking. */
	UPROPERTY(Category = "Movement Info", EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float m_maxSpeed;

	/**
	 * If true, walking movement always maintains horizontal velocity when moving up ramps, which causes movement up ramps to be faster parallel to the ramp surface.
	 * If false, then walking movement maintains velocity magnitude parallel to the ramp surface.
	 */
	UPROPERTY(Category = "Movement Info", EditAnywhere, BlueprintReadWrite)
	uint8 bMaintainHorizontalGroundVelocity : 1;

	/**
	 * Max angle in degrees of a walkable surface. Any greater than this and it is too steep to be walkable.
	 */
	UPROPERTY(Category = "Movement Info", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0", ForceUnits = "degrees"))
	float m_walkableFloorAngle;

	/**
	 * Minimum Z value for floor normal. If less, not a walkable surface. Computed from WalkableFloorAngle.
	 */
	UPROPERTY(Category = "Movement Info", VisibleAnywhere)
	float m_walkableFloorZ;

	/** Maximum height character can step up */
	UPROPERTY(Category = "Movement Info", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm"))
	float m_maxStepHeight;


	/*---------------------------------- FUNCTIONS ----------------------------------*/
protected:
	/**
	 * Adjusts velocity when walking so that Z velocity is zero.
	 * When bMaintainHorizontalGroundVelocity is false, also rescales the velocity vector to maintain the original magnitude, but in the horizontal direction.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "MaintainHorizontalGroundVelocity"))
	void MaintainHorizontalGroundVelocity(FVector& Velocity);
	virtual void MaintainHorizontalGroundVelocity_Implementation(FVector& Velocity);

	/**
	 * Move along the floor, using CurrentFloor and ComputeGroundMovementDelta() to get a movement direction.
	 * If a second walkable surface is hit, it will also be moved along using the same approach.
	 *
	 * @param InVelocity:			Velocity of movement
	 * @param DeltaSeconds:			Time over which movement occurs
	 * @param OutStepDownResult:	[Out] If non-null, and a floor check is performed, this will be updated to reflect that result.
	 */
	void MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FPMFindFloorResult& CurrentFloor, FPMStepDownResult* OutStepDownResult = NULL);

	UFUNCTION()
	virtual FVector ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const;

	/** Return true if the hit result should be considered a walkable surface for the character. */
	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents")
	bool IsWalkable(const FHitResult& Hit) const;
	virtual bool IsWalkable_Implementation(const FHitResult& Hit) const;

	/** Returns true if we can step up on the actor in the given FHitResult. */
	virtual bool CanStepUp(const FHitResult& Hit) const;

	/**
	 * Move up steps or slope. Does nothing and returns false if CanStepUp(Hit) returns false.
	 *
	 * @param GravDir			Gravity vector direction (assumed normalized or zero)
	 * @param Delta				Requested move
	 * @param Hit				[In] The hit before the step up.
	 * @param OutStepDownResult	[Out] If non-null, a floor check will be performed if possible as part of the final step down, and it will be updated to reflect this result.
	 * @return true if the step up was successful.
	 */
	virtual bool StepUp(const FVector& GravDir, const FVector& Delta, const FHitResult& Hit, FPMStepDownResult* OutStepDownResult = NULL);

	/** Custom version of SlideAlongSurface that handles different movement modes separately; namely during walking physics we might not want to slide up slopes. else call move comp slide */
	float SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact);

	/**
	 * Return true if the 2D distance to the impact point is inside the edge tolerance (CapsuleRadius minus a small rejection threshold).
	 * Useful for rejecting adjacent hits when finding a floor or landing spot.
	 */
	virtual bool IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const;

public:
	UPMMovement_Walk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	virtual bool PhysMovement_Implementation(float DeltaTime, int32 Iterations, UPMCharacterMovement* MoveComp) override;
	virtual bool StartMovement_Implementation(float DeltaTime, const UPMCharacterMovement* MoveComp) override;
	virtual float GetMaxSpeed_Implementation() override { return m_maxSpeed; };
};
