// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PMScoreSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API UPMScoreSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
	/**************************** FUNCTION ******************************/
	/**************************** OVERRIDE ******************************/
public:
	bool ShouldCreateSubsystem (UObject* Outer) const override;
};
