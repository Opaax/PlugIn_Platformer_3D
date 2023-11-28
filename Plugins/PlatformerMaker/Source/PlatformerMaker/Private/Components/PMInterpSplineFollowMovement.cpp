// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Components/PMInterpSplineFollowMovement.h"
#include "PlatformerMaker.h"

//Unreal


const float UPMInterpSplineFollowMovement::MIN_TICK_TIME = 0.0002f;

UPMInterpSplineFollowMovement::UPMInterpSplineFollowMovement(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	bUpdateOnlyIfRendered = false;
	bForceSubStepping = false;

	bWantsInitializeComponent = true;
	bAutoRegisterPhysicsVolumeUpdates = false;
	bComponentShouldUpdatePhysicsVolume = false;

	MaxSimulationTimeStep = 0.05f;
	//MaxSimulationIterations = 8;

	//bIsWaiting = false;
	m_timeMultiplier = 1.0f;
	m_duration = 5.f;
	m_currentDirection = 1;
	//bStopped = false;
	//bPointsFinalized = false;
}

void UPMInterpSplineFollowMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CanBeginUpdateMovement(DeltaTime)) 
	{
		return;
	}

	float lRemainingTime = DeltaTime;
	int32 lIterations = 0;

	while (lRemainingTime >= MIN_TICK_TIME && (lIterations < MaxSimulationIterations) && IsValid(GetOwner()) && UpdatedComponent != nullptr && IsActive())
	{
		lIterations++;

		const float lTimeTick = bForceSubStepping ? GetSimulationTimeStep(lRemainingTime, lIterations) : lRemainingTime;
		lRemainingTime -= lTimeTick;

		// Calculate the current alpha with this tick iteration
		const float lTargetTime = FMath::Clamp(m_currentTime + ((lTimeTick * m_timeMultiplier) * m_currentDirection), 0.0f, 1.0f);
		FVector MoveDelta = ComputeMoveDelta(lTargetTime);

		// Update velocity
		Velocity = MoveDelta / lTimeTick;

		// Update the rotation on the spline if required
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); //-V595

		// Move the component
	//	if ((bPauseOnImpact == false) && (BehaviourType != EInterpToBehaviourType::OneShot))
	//	{
	//		// If we can bounce, we are allowed to move out of penetrations, so use SafeMoveUpdatedComponent which does that automatically.
	//		SafeMoveUpdatedComponent(MoveDelta, CurrentRotation, bSweep, Hit, TeleportType);
	//	}
	//	else
	//	{
	//		// If we can't bounce, then we shouldn't adjust if initially penetrating, because that should be a blocking hit that causes a hit event and stop simulation.
	//		TGuardValue<EMoveComponentFlags> ScopedFlagRestore(MoveComponentFlags, MoveComponentFlags | MOVECOMP_NeverIgnoreBlockingOverlaps);
	//		MoveUpdatedComponent(MoveDelta, CurrentRotation, bSweep, &Hit, TeleportType);
	//	}
	//	//DrawDebugPoint(GetWorld(), UpdatedComponent->GetComponentLocation(), 16, FColor::White,true,5.0f);
	//	// If we hit a trigger that destroyed us, abort.
	//	if (!IsValid(ActorOwner) || !UpdatedComponent || !IsActive())
	//	{
	//		return;
	//	}

	//	// Update current time
	//	float AlphaRemainder = 0.0f;
	//	if (bIsWaiting == false)
	//	{
	//		// Compute time used out of tick time to get to the hit
	//		const float TimeDeltaAtHit = TimeTick * Hit.Time;
	//		// Compute new time lerp alpha based on how far we moved
	//		CurrentTime = CalculateNewTime(CurrentTime, TimeDeltaAtHit, Hit, true, bStopped, AlphaRemainder);
	//	}

	//	// See if we moved at all
	//	if (Hit.Time != 0.f)
	//	{
	//		// If we were 'waiting' we are not any more - broadcast we are moving again
	//		if (bIsWaiting == true)
	//		{
	//			OnWaitEndDelegate.Broadcast(Hit, CurrentTime);
	//			bIsWaiting = false;
	//		}
	//	}

	//	// Handle hit result after movement
	//	float SubTickTimeRemaining = 0.0f;
	//	if (!Hit.bBlockingHit)
	//	{
	//		if (bStopped == true)
	//		{
	//			Velocity = FVector::ZeroVector;
	//			break;
	//		}

	//		// Handle remainder of alpha after it goes off the end, for instance if ping-pong is set and it hit the end,
	//		// continue with the time remaining off the end but in the reverse direction. It is similar to hitting an object in this respect.
	//		if (AlphaRemainder != 0.0f)
	//		{
	//			NumBounces++;
	//			SubTickTimeRemaining = (AlphaRemainder * Duration);
	//		}
	//	}
	//	else
	//	{
	//		if (HandleHitWall(Hit, TimeTick, MoveDelta))
	//		{
	//			break;
	//		}

	//		NumBounces++;
	//		SubTickTimeRemaining = TimeTick * (1.f - Hit.Time);
	//	}

	//	// A few initial bounces should add more time and iterations to complete most of the simulation.
	//	if (NumBounces <= 2 && SubTickTimeRemaining >= MIN_TICK_TIME)
	//	{
	//		RemainingTime += SubTickTimeRemaining;
	//		Iterations--;
	//	}
	//}

	UpdateComponentVelocity();
}

void UPMInterpSplineFollowMovement::BeginPlay()
{
	Super::BeginPlay();
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
	return bForceSubStepping;
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
			// If this is the last iteration, just use all the remaining time. This is usually better than cutting things short, as the simulation won't move far enough otherwise.
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

	return lMoveDelta;
}
