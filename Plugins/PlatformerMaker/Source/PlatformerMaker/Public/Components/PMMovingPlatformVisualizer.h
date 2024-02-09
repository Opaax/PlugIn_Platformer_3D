// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PMSplinePathVisualizerComponent.h"
#include "PMMovingPlatformVisualizer.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLATFORMERMAKER_API UPMSplinePointPathVisualizer : public UPMSplinePathVisualizerComponent
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
	/**************************** FUNCTION ******************************/
protected:
	virtual void SetupMeshVisualization_Implementation() override;

public:
	UPMSplinePointPathVisualizer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
};
