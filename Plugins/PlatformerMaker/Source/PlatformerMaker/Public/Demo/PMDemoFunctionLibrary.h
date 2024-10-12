// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlatformerMaker/Public/Demo/Structs/LevelData.h"
#include "PMDemoFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API UPMDemoFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
public:
	/** Breaks an DynamicCameraEffectHandle */
	UFUNCTION(BlueprintPure, Category = "Level Data", meta = (NativeBreakFunc))
	static void BreakLevelData(FLevelData InLevelData, FName& OutName, TSoftObjectPtr<UWorld>& OutLevelPath);
};
