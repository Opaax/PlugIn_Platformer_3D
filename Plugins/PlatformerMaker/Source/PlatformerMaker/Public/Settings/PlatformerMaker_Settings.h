// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlatformerMaker_Settings.generated.h"

/**
 * 
 */
UCLASS(config = PlatformerMaker_Settings)
class PLATFORMERMAKER_API UPlatformerMaker_Settings : public UObject
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
public:
	UPROPERTY(Config, EditAnywhere, Category = "PlatformerMaker")
	bool SpawnScoreSubSystem;

	/**************************** FUNCTION ******************************/
public:
	UPlatformerMaker_Settings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
};
