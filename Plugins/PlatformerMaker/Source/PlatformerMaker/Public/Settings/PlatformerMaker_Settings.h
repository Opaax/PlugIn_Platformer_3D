// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlatformerMaker_Settings.generated.h"

class UPMUW_LoadingBase;

/**
 * Settings for the Plugin Platformer Maker
 */
UCLASS(config = PlatformerMaker_Settings)
class PLATFORMERMAKER_API UPlatformerMaker_Settings : public UObject
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
public:
	UPROPERTY(Config, EditAnywhere, Category = "PlatformerMaker")
	bool SpawnScoreSubSystem;

	UPROPERTY(Config, EditAnywhere, Category = "PlatformerMaker")
	bool SpawnCheckpointSubSystem;

	UPROPERTY(Config, EditAnywhere, Category = "PlatformerMaker")
	bool CreateLoadingSubsystem;

	UPROPERTY(Config, EditAnywhere, Category = "PlatformerMaker")
	TSubclassOf<UPMUW_LoadingBase> BaseLoadingWidget;

	/**************************** FUNCTION ******************************/
public:
	UPlatformerMaker_Settings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
};
