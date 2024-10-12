// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/PM_PlayerCameraManagerDemo.h"
#include "Demo/PM_CameraDemo.h"
#include "Demo/Data/PM_CameraDefaultSettingData.h"

//Unreal
#include <Kismet/KismetMathLibrary.h>
#include "Engine/World.h"

APM_PlayerCameraManagerDemo::APM_PlayerCameraManagerDemo(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	m_cameraDefaultSettings = CreateDefaultSubobject<UPM_CameraDefaultSettingData>(TEXT("CameraDefaultSettings"));
}

FVector APM_PlayerCameraManagerDemo::GetSpawnLocationBaseOnDefaultSetting(FVector& PawnLocation, FVector& PawnRightVector)
{
	if (!m_cameraDefaultSettings) {
		return FVector();
	}

	float lDist = m_cameraDefaultSettings->DistanceToTarget;
	FVector lLocation;

	switch (m_cameraDefaultSettings->ViewSide) {
		case ECameraViewSide::ECV_Right:
			lLocation = PawnLocation + (PawnRightVector * lDist);
			break;
		case ECameraViewSide::ECV_Left:
			lDist *= -1;
			lLocation = PawnLocation + (PawnRightVector * lDist);
			break;
		case ECameraViewSide::ECV_Invalid:
		default:
			lLocation = FVector(50, 50, 50);
			break;
	}

	return lLocation;
}

void APM_PlayerCameraManagerDemo::SpawnCameraDemo(APawn* InPawnToLook)
{
	UWorld* lWorld = GetWorld();

	if (IsValid(lWorld) && IsValid(InPawnToLook)) {

		FVector lPawnLoc = InPawnToLook->GetActorLocation();
		FVector lPawnRight = InPawnToLook->GetActorRightVector();

		const FVector& lSpawnLocation = GetSpawnLocationBaseOnDefaultSetting(lPawnLoc, lPawnRight);
		const FRotator lSpawnRotation = UKismetMathLibrary::FindLookAtRotation(lSpawnLocation, lPawnLoc);
		const FTransform lSpawnTransform = FTransform(lSpawnRotation, lSpawnLocation, FVector::OneVector);

		m_currentCamera = lWorld->SpawnActor<APM_CameraDemo>(APM_CameraDemo::StaticClass(), lSpawnTransform);

		if (IsValid(m_currentCamera)) {

			m_currentCamera->SetSettings(m_cameraDefaultSettings);
			m_currentCamera->SetPawnToFollow(InPawnToLook);

			SetViewTarget(m_currentCamera);
		}
	}
}

void APM_PlayerCameraManagerDemo::DestroyCameraDemo()
{
	if (m_currentCamera) {
		m_currentCamera->Destroy();
	}
}
