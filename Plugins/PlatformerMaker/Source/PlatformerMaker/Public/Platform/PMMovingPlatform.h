// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Platform/PMPlatform.h"
#include "PMMovingPlatform.generated.h"

class USplineComponent;
class UInterpToMovementComponent;

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
	TObjectPtr<UInterpToMovementComponent > m_interpToMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (AllowPrivateAccess = "true", DisplayName = "MoveDuration"))
	float m_moveDuration = 2.f;

	/**************************** FUNCTIONS ******************************/
protected:

	UFUNCTION(BlueprintCallable)
	virtual void SetInterpPoints();

	UFUNCTION(BlueprintCallable)
	virtual void FinalizeInterpComp();

public:
	APMMovingPlatform(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
};
