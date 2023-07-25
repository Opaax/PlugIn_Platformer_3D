// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Platform/PMPlatform.h"
#include "PMMovingPlatform.generated.h"

class USplineComponent;
class UPMInterpToMovementComponent;

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API APMMovingPlatform : public APMPlatform
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> m_splineComponent;

	/*
	* this component work with spline
	* BUT it do not care about tangent. Add more points on spline to make curves
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPMInterpToMovementComponent> m_interpToMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "MoveDuration"))
	float m_moveDuration = 2.f;

	/**************************** FUNCTIONS ******************************/
protected:

	UFUNCTION(BlueprintCallable, Category = "PMMovingPlatform")
	virtual void SetInterpPoints();

	UFUNCTION(BlueprintCallable, Category = "PMMovingPlatform")
	virtual void FinalizeInterpComp();

public:
	APMMovingPlatform(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
};
