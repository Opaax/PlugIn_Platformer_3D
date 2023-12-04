// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "PMInterpSplineFollowMovement.generated.h"

class USplineComponent;

UENUM()
enum class EPMInterpSplineFollowMovement : uint8
{
	EIFM_OneShoot UMETA(DisplayName = "One Shoot"),
	EIFM_LoopRestart UMETA(DisplayName = "Loop Restart"),
	EIFM_PingPongLoop UMETA(DisplayName = "Ping Pong Loop"),
};

/**
 * Move the root component along a spline over a given time
 */
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), HideCategories = Velocity)
class PLATFORMERMAKER_API UPMInterpSplineFollowMovement : public UMovementComponent
{
	GENERATED_BODY()
	
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

	/* The behavior that the component will be moved*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PM|Setting", meta = (DisplayName = "BehaviorType"))
	EPMInterpSplineFollowMovement m_behaviorType;

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

	/** Physics teleport type. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behaviour)
	ETeleportType m_teleportType = ETeleportType::None;

	/** If true, will sweep for blocking collision during movement. If false, it will simply teleport to the next position and ignore collision. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behaviour)
	bool bSweep = true;

	/** If true, will pause movement on impact. If false it will behave as if the end of the movement range was reached based on the BehaviourType. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behaviour)
	uint8 bPauseOnImpact;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintGetter = "GetSpline", BlueprintSetter = "SetSpline", Category = "PM|Refs")
	TObjectPtr<USplineComponent> m_splineTarget;

	float TotalDistance;

	/* Current position on spline */
	float m_currentTime;

	int32 m_currentDirection;

	FVector StartLocation;

	/* Have we hit something and are waiting for it to move to carry on moving */
	bool bIsWaiting;

	/* Have we stopped (because we hit something, or reached the end of the cycle */
	bool bStopped;
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

	/** @return true if the simulation should stop. */
	bool HandleHitWall(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta);

	/** Deal with an impact. Change direction, stop etc depending on the current behaviour setting. */
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;

	/** Compute the distance for the given time. */
	virtual FVector ComputeMoveDelta(float Time) const;

	/* Calculate the new current time */
	virtual float CalculateNewTime(float TimeNow, float Delta, FHitResult& HitResult, bool InBroadcastEvent, bool& OutStopped, float& OutTimeRemainder);

	/** Clears the reference to UpdatedComponent, fires stop event, and stops ticking. */
	UFUNCTION(BlueprintCallable, Category = "Game|Components|InterpSplineMovement")
	void StopSimulating();
public:
	UPMInterpSplineFollowMovement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE USplineComponent* GetSpline() const { return m_splineTarget; }

	UFUNCTION(BlueprintCallable)
	void SetSpline(USplineComponent* InSpline);
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
