// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "PMAIPerceptionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLATFORMERMAKER_API UPMAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()
	
protected:
	virtual void OnRegister() override;

};
