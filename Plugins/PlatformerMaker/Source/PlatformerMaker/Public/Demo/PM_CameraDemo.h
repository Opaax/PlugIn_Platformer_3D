// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "../Demo/PMCameraEnums.h"
#include "PM_CameraDemo.generated.h"

class APawn;
class UPM_CameraDefaultSettingData;

/**
 * Camera for Demo
 * 
 * Side follow
 */
UCLASS()
class PLATFORMERMAKER_API APM_CameraDemo : public ACameraActor
{
	GENERATED_UCLASS_BODY()

	/*---------------------------------- MEMBERS ----------------------------------*/
protected:
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<APawn> m_currentPawn;

	float m_distanceToTarget;
	float m_defaultDistanceToTarget;
	float m_forwardOffset;
	float m_upwardOffset;
	ECameraViewSide m_viewSide;
	FRotator m_pawnBaseRotation;

	/*---------------------------------- FUNCTION ----------------------------------*/
protected:
	UFUNCTION(Category = "CameraDemo")
	void UpdateLocation(float DeltaTime);

public:
	UFUNCTION(Category = "CameraDemo")
	void SetPawnToFollow(APawn* InPawn);

	UFUNCTION(Category = "CameraDemo")
	void SetSettings(UPM_CameraDefaultSettingData* InSettings);

	UFUNCTION(Category = "CameraDemo", BlueprintCallable)
	void SetDistanceToTarget(const float NewDistance);

	UFUNCTION(Category = "CameraDemo", BlueprintCallable)
	void ResetDistanceToTarget();

	UFUNCTION(Category = "CameraDemo", BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetDistanceToTarget() const { return m_distanceToTarget; };

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	virtual void Tick(float DeltaSeconds) override;
};
