// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PM_PlayerCameraManagerDemo.generated.h"

class APM_CameraDemo;
class UPM_CameraDefaultSettingData;

/**
 * Camera manager for the demo
 */
UCLASS()
class PLATFORMERMAKER_API APM_PlayerCameraManagerDemo : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
protected:

	UPROPERTY(VisibleInstanceOnly, Category = "Camera|Settings", meta = (DisplayName = "CameraDefaultSettings"))
	TObjectPtr<UPM_CameraDefaultSettingData> m_cameraDefaultSettings;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime",BlueprintGetter = "GetCurrentCamera", meta = (DisplayName = "CurrentCamera"))
	TObjectPtr<APM_CameraDemo> m_currentCamera;

	/*---------------------------------- FUNCTION ----------------------------------*/
private:
	UFUNCTION()
	FVector GetSpawnLocationBaseOnDefaultSetting(FVector& PawnLocation, FVector& PawnRightVector);

public:
	UFUNCTION(Category = "DemoCameraManager")
	void SpawnCameraDemo(APawn* InPawnToLook);

	UFUNCTION(Category = "DemoCameraManager")
	void DestroyCameraDemo();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DemoCameraManager")
	FORCEINLINE APM_CameraDemo* GetCurrentCamera() const { return m_currentCamera; }

	/*---------------------------------- OVERRIDE ----------------------------------*/

};
