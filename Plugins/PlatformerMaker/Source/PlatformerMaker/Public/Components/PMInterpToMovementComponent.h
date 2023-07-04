// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/InterpToMovementComponent.h"
#include "PMInterpToMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API UPMInterpToMovementComponent : public UInterpToMovementComponent
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
	/**************************** FUNCTION ******************************/
public:

	UFUNCTION(BlueprintCallable)
	void RestartForReverse();

	UFUNCTION(BlueprintCallable)
	void RestartForNormal();

	/**************************** OVERRIDE ******************************/
	
};
