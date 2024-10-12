// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../PMCameraEnums.h"
#include "PM_CameraDefaultSettingData.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API UPM_CameraDefaultSettingData : public UDataAsset
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMEBRS ----------------------------------*/
public:
	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float DistanceToTarget = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float ForwardOffset = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float UpwardOffset = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	ECameraViewSide ViewSide = ECameraViewSide::ECV_Right;
};
