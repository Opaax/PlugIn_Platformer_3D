//2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Components/PMInterpSplineFollowMovement.h"
#include "PlatformerMaker.h"
#include "Utils/DebugMacro.h"

//Unreal
#include "Components/SplineComponent.h"
#include "Components/ShapeComponent.h"

const float UPMInterpSplineFollowMovement::MIN_TICK_TIME = 0.0002f;

UPMInterpSplineFollowMovement::UPMInterpSplineFollowMovement(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	bUpdateOnlyIfRendered = false;
	bForceSubStepping = false;

	bWantsInitializeComponent = true;
	bAutoRegisterPhysicsVolumeUpdates = false;
	bComponentShouldUpdatePhysicsVolume = false;
	m_teleportType = ETeleportType::None;

	MaxSimulationTimeStep = 0.05f;
	MaxSimulationIterations = 8;

	bIsWaiting = false;
	m_timeMultiplier = 1.0f;
	m_duration = 5.f;
	m_currentDirection = 1;
	bStopped = false;
	bPauseOnImpact = true;
	bUseSplineRotation = false;
}

void UPMInterpSplineFollowMovement::SetSpline(USplineComponent* InSpline)
{
	m_splineTarget = InSpline;

	if (m_splineTarget == nullptr || !IsValid(UpdatedComponent))
	{
		return;
	}

	const FVector& lStartLoc = m_splineTarget->GetLocationAtSplineInputKey(0, ESplineCoordinateSpace::World);

	UpdatedComponent->SetWorldLocation(lStartLoc);
}

void UPMInterpSplineFollowMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CanBeginUpdateMovement(DeltaTime)) 
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("Can't begin Update movement"));

		return;
	}

	float lRemainingTime = DeltaTime;
	int32 lIterations = 0;
	int32 lNumBounces = 0;
	FHitResult lHit(1.f);

	while (lRemainingTime >= MIN_TICK_TIME && (lIterations < MaxSimulationIterations) && IsValid(GetOwner()) && UpdatedComponent != nullptr && IsActive())
	{
		lIterations++;

		const float lTimeTick = bForceSubStepping ? GetSimulationTimeStep(lRemainingTime, lIterations) : lRemainingTime;
		lRemainingTime -= lTimeTick;

		// Calculate the current alpha with this tick iteration
		const float lTargetTime = FMath::Clamp(m_currentTime + ((lTimeTick * m_timeMultiplier) * m_currentDirection), 0.0f, m_duration);
		FVector lMoveDelta = ComputeMoveDelta(lTargetTime);

		// Update velocity
		Velocity = lMoveDelta / lTimeTick;

		// Update the rotation on the spline if required
		FRotator lCurrentRotation = UpdatedComponent->GetComponentRotation();

		if (bUseSplineRotation) {
			lCurrentRotation = ComputeRotationDelta(lTargetTime);
		}

		// Move the component
		if ((bPauseOnImpact == false) && (m_behaviorType != EPMInterpSplineFollowMovement::EIFM_OneShoot))
		{
			// If we can bounce, we are allowed to move out of penetrations, so use SafeMoveUpdatedComponent which does that automatically.
			SafeMoveUpdatedComponent(lMoveDelta, lCurrentRotation, bSweep, lHit, m_teleportType);
		}
		else
		{
			// If we can't bounce, then we shouldn't adjust if initially penetrating, because that should be a blocking hit that causes a hit event and stop simulation.
			TGuardValue<EMoveComponentFlags> ScopedFlagRestore(MoveComponentFlags, MoveComponentFlags | MOVECOMP_NeverIgnoreBlockingOverlaps);
			MoveUpdatedComponent(lMoveDelta, lCurrentRotation, bSweep, &lHit, m_teleportType);
		}

		// If we hit a trigger that destroyed us, abort.
		if (!UpdatedComponent || !IsValid(UpdatedComponent->GetOwner()) || !IsActive())
		{
			return;
		}

		// Update current time
		float lAlphaRemainder = 0.0f;

		if (bIsWaiting == false)
		{
			// Compute time used out of tick time to get to the hit
			const float TimeDeltaAtHit = lTimeTick * lHit.Time;
			// Compute new time lerp alpha based on how far we moved
			m_currentTime = CalculateNewTime(m_currentTime, TimeDeltaAtHit, lHit, true, bStopped, lAlphaRemainder);
		}

		// See if we moved at all
		if (lHit.Time != 0.f)
		{
			// If we were 'waiting' we are not any more - broadcast we are moving again
			if (bIsWaiting == true)
			{
				bIsWaiting = false;
			}
		}

		// Handle hit result after movement
		float SubTickTimeRemaining = 0.0f;
		if (!lHit.bBlockingHit)
		{
			if (bStopped == true)
			{
				Velocity = FVector::ZeroVector;
				break;
			}

			// Handle remainder of alpha after it goes off the end, for instance if ping-pong is set and it hit the end,
			// continue with the time remaining off the end but in the reverse direction. It is similar to hitting an object in this respect.
			if (lAlphaRemainder != 0.0f)
			{
				lNumBounces++;
				SubTickTimeRemaining = (lAlphaRemainder * m_duration);
			}
		}
		else
		{
			if (HandleHitWall(lHit, lTimeTick, lMoveDelta))
			{
				break;
			}

			lNumBounces++;
			SubTickTimeRemaining = lTimeTick * (1.f - lHit.Time);
		}

		// A few initial bounces should add more time and iterations to complete most of the simulation.
		if (lNumBounces <= 2 && SubTickTimeRemaining >= MIN_TICK_TIME)
		{
			lRemainingTime += SubTickTimeRemaining;
			lIterations--;
		}
	}

	UpdateComponentVelocity();
}

void UPMInterpSplineFollowMovement::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	CheckOwnerRootComp();
#endif
}

void UPMInterpSplineFollowMovement::StopMovementImmediately()
{
	Super::StopMovementImmediately();
}

bool UPMInterpSplineFollowMovement::CanBeginUpdateMovement(const float& DeltaTime)
{
	// skip if don't want component updated when not rendered or updated component can't move
	if (UpdatedComponent == nullptr|| ShouldSkipUpdate(DeltaTime) || m_splineTarget == nullptr)
	{
		return false;
	}

	AActor* lOwner = UpdatedComponent->GetOwner();

	if (!lOwner || !CheckStillInWorld())
	{
		return false;
	}

	if (bStopped) {
		return false;
	}

	return true;
}

bool UPMInterpSplineFollowMovement::CheckStillInWorld()
{
	if (!UpdatedComponent)
	{
		return false;
	}

	const UWorld* lWorld = GetWorld();

	if (!lWorld)
	{
		return false;
	}

	// check the variations of KillZ
	AWorldSettings* lWorldSettings = lWorld->GetWorldSettings(true);
	if (!lWorldSettings->AreWorldBoundsChecksEnabled())
	{
		return true;
	}

	AActor* lOwner = UpdatedComponent->GetOwner();

	if (!IsValid(lOwner))
	{
		return false;
	}

	if (lOwner->GetActorLocation().Z < lWorldSettings->KillZ)
	{
		UDamageType const* DmgType = lWorldSettings->KillZDamageType ? lWorldSettings->KillZDamageType->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
		lOwner->FellOutOfWorld(*DmgType);

		return false;
	}

	// Check if box has poked outside the world
	else if (UpdatedComponent != nullptr && UpdatedComponent->IsRegistered())
	{
		const FBox& Box = UpdatedComponent->Bounds.GetBox();
		if (Box.Min.X < -HALF_WORLD_MAX || Box.Max.X > HALF_WORLD_MAX ||
			Box.Min.Y < -HALF_WORLD_MAX || Box.Max.Y > HALF_WORLD_MAX ||
			Box.Min.Z < -HALF_WORLD_MAX || Box.Max.Z > HALF_WORLD_MAX)
		{
			UE_LOG(LogPlatformerPlugin, Warning, TEXT("%s is outside the world bounds!"), *GetNameSafe(lOwner));
			lOwner->OutsideWorldBounds();

			// not safe to use physics or collision at this point
			lOwner->SetActorEnableCollision(false);

			/*FHitResult Hit(1.f);
			StopSimulating(Hit);*/

			return false;
		}
	}
	return true;
}

bool UPMInterpSplineFollowMovement::ShouldUseSubStepping() const
{
	return (bool)bForceSubStepping;
}

float UPMInterpSplineFollowMovement::GetSimulationTimeStep(float RemainingTime, int32 Iterations) const
{
	if (RemainingTime > MaxSimulationTimeStep)
	{
		if (Iterations < MaxSimulationIterations)
		{
			// Subdivide moves to be no longer than MaxSimulationTimeStep seconds
			RemainingTime = FMath::Min(MaxSimulationTimeStep, RemainingTime * 0.5f);
		}
		else
		{
			// If this is the last iteration, just use all the remaining time. 
			// This is usually better than cutting things short, as the simulation won't move far enough otherwise.
			// Print a throttled warning.
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			static uint32 s_WarningCount = 0;
			if ((s_WarningCount++ < 100) || (GFrameCounter & 15) == 0)
			{
				UE_LOG(LogPlatformerPlugin, Warning, TEXT("GetSimulationTimeStep() - Max iterations %d hit while remaining time %.6f > MaxSimulationTimeStep (%.3f) for '%s'"), MaxSimulationIterations, RemainingTime, MaxSimulationTimeStep, *GetPathNameSafe(UpdatedComponent));
			}
#endif
		}
	}

	// no less than MIN_TICK_TIME (to avoid potential divide-by-zero during simulation).
	return FMath::Max(MIN_TICK_TIME, RemainingTime);
}

FVector UPMInterpSplineFollowMovement::ComputeMoveDelta(float Time) const
{
	FVector lMoveDelta = FVector::ZeroVector;
	FVector lNewPosition = FVector::ZeroVector;

	//Cache Current Update Comp
	const FVector& lCurrentPosition = UpdatedComponent->GetComponentLocation();
	//Compute next alpha 
	float lAlpha = FMath::Clamp(Time / m_duration, 0.f, 1.f);
	//Compute spline Alpha
	float lAlphaSpline = FMath::Lerp(0.f, 1.f, lAlpha);

	lNewPosition = m_splineTarget->GetLocationAtTime(lAlphaSpline, ESplineCoordinateSpace::World);
	lMoveDelta = lNewPosition - lCurrentPosition;

	return lMoveDelta;
}

FRotator UPMInterpSplineFollowMovement::ComputeRotationDelta(float Time) const
{
	FRotator lRotDelta = FRotator();
	FRotator lNewRot = FRotator();

	//Cache Current Update Comp
	const FRotator& lCurrentRot = UpdatedComponent->GetComponentRotation();
	//Compute next alpha 
	float lAlpha = FMath::Clamp(Time / m_duration, 0.f, 1.f);
	//Compute spline Alpha
	float lAlphaSpline = FMath::Lerp(0.f, 1.f, lAlpha);

	lNewRot = m_splineTarget->GetRotationAtTime(lAlphaSpline, ESplineCoordinateSpace::World);
	lRotDelta = lNewRot;

	return lRotDelta;
}

float UPMInterpSplineFollowMovement::CalculateNewTime(float TimeNow, float Delta, FHitResult& HitResult, bool InBroadcastEvent, bool& OutStopped, float& OutTimeRemainder)
{
	float lNewTime = TimeNow;

	lNewTime += ((Delta * m_timeMultiplier) * m_currentDirection);

	switch (m_behaviorType)
	{
	case EPMInterpSplineFollowMovement::EIFM_OneShoot:
		if (lNewTime >= m_duration)
		{
			lNewTime = m_duration;
			bStopped = true;
		}
		break;
	case EPMInterpSplineFollowMovement::EIFM_LoopRestart:
		if (lNewTime >= m_duration)
		{
			lNewTime = 0;
		}
		break;
	case EPMInterpSplineFollowMovement::EIFM_PingPongLoop:
		if (lNewTime >= m_duration)
		{
			lNewTime = m_duration;
			m_currentDirection = -1;
		}
		else if (lNewTime < 0) {
			lNewTime = 0;
			m_currentDirection = 1;
		}
		break;
	default:
		break;
	}
	
	return lNewTime;
}

bool UPMInterpSplineFollowMovement::HandleHitWall(const FHitResult& Hit, float Time, const FVector& MoveDelta)
{
	AActor* lActorOwner = UpdatedComponent ? UpdatedComponent->GetOwner() : NULL;
	if (!CheckStillInWorld() || !IsValid(lActorOwner))
	{
		return true;
	}

	HandleImpact(Hit, Time, MoveDelta);

	if (!IsValid(lActorOwner) || !UpdatedComponent)
	{
		return true;
	}

	return false;
}

void UPMInterpSplineFollowMovement::HandleImpact(const FHitResult& Hit, float Time, const FVector& MoveDelta)
{
	if (bPauseOnImpact == false)
	{
		switch (m_behaviorType)
		{
		case EPMInterpSplineFollowMovement::EIFM_OneShoot:
			//OnInterpToStop.Broadcast(Hit, Time);
			bStopped = true;
			StopSimulating();
			return;
		case EPMInterpSplineFollowMovement::EIFM_LoopRestart:
		{
			m_currentTime = 0.0f;
			//OnResetDelegate.Broadcast(Hit, CurrentTime);
		}
		break;
		default:
			//ReverseDirection(Hit, Time, true);
			m_currentDirection = -m_currentDirection;
			break;
		}
	}
	else
	{
		if (bIsWaiting == false)
		{
			//OnWaitBeginDelegate.Broadcast(Hit, Time);
			bIsWaiting = true;
		}
	}
}

void UPMInterpSplineFollowMovement::StopSimulating()
{
	SetUpdatedComponent(nullptr);
	Velocity = FVector::ZeroVector;
	bStopped = true;
	//OnInterpToStop.Broadcast(HitResult, CurrentTime);
}

void UPMInterpSplineFollowMovement::CheckOwnerRootComp()
{
	if (IsValid(GetOwner()) && IsValid(GetOwner()->GetRootComponent())) {
		if (!GetOwner()->GetRootComponent()->IsA(UShapeComponent::StaticClass()) && bPauseOnImpact) {
			UE_LOG(LogPlatformerPlugin, Warning, TEXT("%s, its owner:%s, hasn't a shape component as root comp. This may handle impact not right"), *GetNameSafe(this), *GetNameSafe(GetOwner()));
		}
	}
}
