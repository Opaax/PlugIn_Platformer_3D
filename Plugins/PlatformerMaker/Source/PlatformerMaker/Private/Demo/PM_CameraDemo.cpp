// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/PM_CameraDemo.h"
#include "Demo/Data/PM_CameraDefaultSettingData.h"

APM_CameraDemo::APM_CameraDemo(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
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
	} else {
		m_viewSide = ECameraViewSide::ECV_Right;
		m_distanceToTarget = 500.f;
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
	const FVector& lPawnRightVec = m_pawnBaseRotation.RotateVector(FVector::RightVector);
	float lDist = m_viewSide == ECameraViewSide::ECV_Right ? m_distanceToTarget : m_distanceToTarget * -1;

	const FVector& lLocationReel = lPawnLocation + (lPawnRightVec * lDist);

	FHitResult lHit(1.f);
	SetActorLocation(lLocationReel, true, &lHit);
}