#pragma once

#include "CoreMinimal.h"
#include "Engine/Level.h"
#include "Engine/DataTable.h"
#include "LevelData.generated.h"

class UWorld;

/** Structure that defines a level up table entry */
USTRUCT(Blueprintable, BlueprintType, meta = (HasNativeBreak = "PlatformerMaker.PMDemoFunctionLibrary.BreakLevelData"))
struct FLevelData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "LevelData")
    FName LevelName;

    UPROPERTY(EditAnywhere, Category= "LevelData")
    TSoftObjectPtr<UWorld> LevelPath;

public:
    FLevelData()
        : LevelName()
        , LevelPath(nullptr)
    {}
};