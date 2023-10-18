// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "PMSplinePathVisualizerComponent.h"

#if WITH_EDITOR
#include "Utils/DebugMacro.h"
#endif

//Unreal
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"

UPMSplinePathVisualizerComponent::UPMSplinePathVisualizerComponent(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	m_meshVisualization = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("MeshVisualization"));
	m_meshVisualization->NumCustomDataFloats = 1;

	PrimaryComponentTick.bCanEverTick = false;

	m_distanceBetweenPoints = 100.f;
}

void UPMSplinePathVisualizerComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupMeshVisualization();
}

void UPMSplinePathVisualizerComponent::SetupMeshVisualization_Implementation()
{
	if (CanSetupMesh())
	{
		m_meshVisualization->ClearInstances();

		const float& lLength = m_splineInOwner->GetSplineLength();
		const int32 lNumDots = FMath::RoundToInt32(lLength / m_distanceBetweenPoints);

		float lCurrentDist = m_distanceBetweenPoints;
		FVector lCurrentLoc = FVector();
		FTransform lTrans = FTransform();

		for (int32 i = 0; i < lNumDots; i++)
		{
			lCurrentLoc = m_splineInOwner->GetLocationAtDistanceAlongSpline(lCurrentDist, ESplineCoordinateSpace::World);
			lTrans = FTransform(FQuat::Identity,lCurrentLoc, m_meshInstanceScale);

			m_meshVisualization->AddInstance(lTrans);
			m_meshVisualization->SetCustomDataValue(i,0, lCurrentDist / lLength);

			lCurrentDist += m_distanceBetweenPoints;

#if WITH_EDITOR
			DRAW_SPHERE_AT(lCurrentLoc, 5.f);
#endif
		}

		m_meshVisualization->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

bool UPMSplinePathVisualizerComponent::CanSetupMesh_Implementation()
{
	if (GetOwner() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner null in %s"), *FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))));
		return false;
	}

	m_splineInOwner = GetOwner()->FindComponentByClass<USplineComponent>();

	if (m_splineInOwner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Spline Component in %s"), *GetNameSafe(this));
		return false;
	}

	return true;
}