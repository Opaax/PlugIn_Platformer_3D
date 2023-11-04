// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PMSplinePathVisualizerComponent.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class USplineComponent;

/*
* Component to visualize the path of the spline while playing
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLATFORMERMAKER_API UPMSplinePathVisualizerComponent : public USceneComponent
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(EditAnywhere, Category = "MeshVisualization|Setting", meta = (DisplayName = "DistanceBetweenPoints"))
	float m_distanceBetweenPoints;

	UPROPERTY(EditAnywhere, Category = "MeshVisualization|Setting", meta = (DisplayName = "MeshInstanceScale"))
	FVector m_meshInstanceScale;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MeshVisualization|Setting", meta = (DisplayName = "MeshVisualization"))
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> m_meshVisualization;

	UPROPERTY(BlueprintReadOnly, Category = "MeshVisualization|Runtime", meta = (DisplayName = "SplineInOwner"))
	TObjectPtr<USplineComponent> m_splineInOwner;

	/**************************** FUNCTION ******************************/
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "MeshVisualization")
	void SetupMeshVisualization();
	virtual void SetupMeshVisualization_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "MeshVisualization")
	bool CanSetupMesh();
	virtual bool CanSetupMesh_Implementation();

public:	
	UPMSplinePathVisualizerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;
};
