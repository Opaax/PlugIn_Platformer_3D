// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Movement/PMMovementMode.h"
#include "PMMovement_Walk.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API UPMMovement_Walk : public UPMMovementMode
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
protected:
	/** The maximum ground speed when walking. */
	UPROPERTY(Category = "Movement Info", EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float m_maxSpeed;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
public:
	UPMMovement_Walk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	virtual bool PhysMovement_Implementation(float DeltaTime, int32 Iterations, UPMCharacterMovement* MoveComp) override;
	virtual bool StartMovement_Implementation(float DeltaTime, const UPMCharacterMovement* MoveComp) override;

};
