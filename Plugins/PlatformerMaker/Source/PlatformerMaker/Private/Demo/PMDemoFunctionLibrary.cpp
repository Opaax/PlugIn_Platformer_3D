// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "PlatformerMaker/Public/Demo/PMDemoFunctionLibrary.h"

UPMDemoFunctionLibrary::UPMDemoFunctionLibrary(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{

}

void UPMDemoFunctionLibrary::BreakLevelData(FLevelData InLevelData, FName& OutName, TSoftObjectPtr<UWorld>& OutLevelPath)
{
	OutName = InLevelData.LevelName;
	OutLevelPath = InLevelData.LevelPath;
}
