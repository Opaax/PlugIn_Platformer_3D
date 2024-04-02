// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/PM_CameraDemo.h"
#include "Demo/Data/PM_CameraDefaultSettingData.h"
#include "Kismet/KismetMathLibrary.h"

APM_CameraDemo::APM_CameraDemo(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	m_viewSide = ECameraViewSide::ECV_Right;
	m_distanceToTarget = 500.f;
	m_forwardOffset = 100.f;
}

void APM_CameraDemo::SetPawnToFollow(APawn* InPawn)
{
	m_currentPawn = InPawn;

	if (m_currentPawn) {
		m_pawnBaseRotation = m_currentPawn->GetActorRotation();
		SetActorTickEnabled(true);
	}
}

void APM_CameraDemo::SetSettings(UPM_CameraDefaultSettingData* InSettings)
{
	if (InSettings) {
		m_viewSide = InSettings->ViewSide;
		m_distanceToTarget = InSettings->DistanceToTarget;
		m_forwardOffset = InSettings->ForwardOffset;
	}
}

void APM_CameraDemo::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateLocation(DeltaSeconds);
}

void APM_CameraDemo::UpdateLocation(float DeltaTime)
{
	if (!m_currentPawn) {
		return;
	}

	const FVector& lPawnLocation = m_currentPawn->GetActorLocation();
	const FVector& lRightVec = m_pawnBaseRotation.RotateVector(FVector::RightVector);
	const FVector& lForwardVec = m_pawnBaseRotation.RotateVector(FVector::ForwardVector);

	float lDistToTarget = m_viewSide == ECameraViewSide::ECV_Right ? m_distanceToTarget : m_distanceToTarget * -1;
	float lFowardOffsetDot = lForwardVec.Dot(m_currentPawn->GetActorForwardVector());
	float lFowardOffsetLoc = lFowardOffsetDot >= 0 ? m_forwardOffset : m_forwardOffset * -1;

	FVector lLocationOffset = lPawnLocation + (lForwardVec * lFowardOffsetLoc);
	FVector lLocationReel = lLocationOffset + (lRightVec * lDistToTarget);
	lLocationReel = UKismetMathLibrary::VInterpTo(GetActorLocation(), lLocationReel, DeltaTime, 10.f);

	FHitResult lHit(1.f);
	SetActorLocation(lLocationReel, true, &lHit);
}