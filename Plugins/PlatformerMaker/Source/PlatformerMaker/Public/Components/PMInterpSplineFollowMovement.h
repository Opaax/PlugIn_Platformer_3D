// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "PMInterpSplineFollowMovement.generated.h"

class USplineComponent;

/**
 * Move the root component along a spline over a given time
 */
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), HideCategories = Velocity)
class PLATFORMERMAKER_API UPMInterpSplineFollowMovement : public UMovementComponent
{
	GENERATED_UCLASS_BODY()
	
	/**************************** STATICS ******************************/
protected:
	/** Minimum delta time considered when ticking. Delta times below this are not considered. This is a very small non-zero positive value to avoid potential divide-by-zero in simulation code. */
	static const float MIN_TICK_TIME;

	/**************************** MEMBERS ******************************/
protected:
	/* How long to take to move along the spline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PM|Setting", meta = (UIMin = 0.1f, ClampMin = 0.1f))
	float m_duration;

	/* User to  aplly some time dilatation*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PM|Setting", meta = (UIMin = 0.1f, ClampMin = 0.1f, DisplayName = "TimeMultiplier"))
	float m_timeMultiplier;

	/**
	 * Max number of iterations used for each discrete simulation step.
	 * Increasing this value can address issues with fast-moving objects or complex collision scenarios, at the cost of performance.
	 *
	 * WARNING: if (MaxSimulationTimeStep * MaxSimulationIterations) is too low for the min framerate, the last simulation step may exceed MaxSimulationTimeStep to complete the simulation.
	 * @see MaxSimulationTimeStep, bForceSubStepping
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "25", UIMin = "1", UIMax = "25"), Category = "PM|Simulation")
	int32 MaxSimulationIterations;

	/**
	 * Max time delta for each discrete simulation step.
	 * Lowering this value can address issues with fast-moving objects or complex collision scenarios, at the cost of performance.
	 *
	 * WARNING: if (MaxSimulationTimeStep * MaxSimulationIterations) is too low for the min framerate, the last simulation step may exceed MaxSimulationTimeStep to complete the simulation.
	 * @see MaxSimulationIterations, bForceSubStepping
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0166", ClampMax = "0.50", UIMin = "0.0166", UIMax = "0.50"), Category = "PM|Simulation")
	float MaxSimulationTimeStep;

	/**
	 * If true, forces sub-stepping to break up movement into discrete smaller steps to improve accuracy of the trajectory.
	 * Objects that move in a straight line typically do *not* need to set this, as movement always uses continuous collision detection (sweeps) so collision is not missed.
	 * Sub-stepping is automatically enabled when under the effects of gravity or when homing towards a target.
	 * @see MaxSimulationTimeStep, MaxSimulationIterations
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PM|Simulation")
	uint8 bForceSubStepping;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PM|Refs")
	TObjectPtr<USplineComponent> m_splineTarget;

	float TotalDistance;

	/* Current position on spline */
	float m_currentTime;

	int32 m_currentDirection;

	FVector StartLocation;
	/**************************** FUNCTION ******************************/
protected:

	/**
	* Make all check needed in order to make sure its safe to start the update
	*/
	virtual bool CanBeginUpdateMovement(const float& DeltaTime);

	/**
	 * This will check to see if the InterpTo is still in the world.  It will check things like
	 * the KillZ, outside world bounds, etc. and handle the situation.
	 */
	virtual bool CheckStillInWorld();

	/**
	 * Determine whether or not to use substepping in the InterpTo motion update.
	 * If true, GetSimulationTimeStep() will be used to time-slice the update. If false, all remaining time will be used during the tick.
	 * @return Whether or not to use substepping in the InterpTo motion update.
	 * @see GetSimulationTimeStep()
	 */
	virtual bool ShouldUseSubStepping() const;

	/**
	 * Compute remaining time step given remaining time and current iterations.
	 * The last iteration (limited by MaxSimulationIterations) always returns the remaining time, which may violate MaxSimulationTimeStep.
	 *
	 * @param RemainingTime		Remaining time in the tick.
	 * @param Iterations		Current iteration of the tick (starting at 1).
	 * @return The remaining time step to use for the next sub-step of iteration.
	 * @see MaxSimulationTimeStep, MaxSimulationIterations
	 * @see ShouldUseSubStepping()
	 */
	float GetSimulationTimeStep(float RemainingTime, int32 Iterations) const;

	/** Compute the distance for the given time. */
	virtual FVector ComputeMoveDelta(float Time) const;

public:
	UPMInterpSplineFollowMovement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/**************************** OVERRIDE ******************************/
protected:
	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	//End UActorComponent Interface

	//Begin UMovementComponent Interface
	virtual void StopMovementImmediately() override;
	//End UMovementComponent Interface
};
