// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Components/PMMovingPlatformVisualizer.h"
#include "Utils/DebugMacro.h"

//Unreal
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"

UPMSplinePointPathVisualizer::UPMSplinePointPathVisualizer(const FObjectInitializer& ObjectInitializer):Super()
{
}

void UPMSplinePointPathVisualizer::SetupMeshVisualization_Implementation()
{
	if (CanSetupMesh()) 
	{
		m_meshVisualization->ClearInstances();

		const int32 lPointNum = m_splineInOwner->GetNumberOfSplinePoints();

		float lLengthTo = 0;
		float lCurrentDist = 0;
		float lRatio = 0;

		int32 lNumDots = 0;
		int32 lTotalIndex = 0;
		int32 lIndexStart = 0;
		int32 lIndexEnd = 0;

		FVector lStart = FVector();
		FVector lEnd = FVector();
		FVector lCurrentLoc = FVector();
		FTransform lTrans = FTransform();

		for (int32 i = 0; i < lPointNum - 1; i++)
		{
			lIndexStart = i;
			lIndexEnd = i + 1;

			lStart = m_splineInOwner->GetLocationAtSplinePoint(lIndexStart, ESplineCoordinateSpace::World);
			lEnd = m_splineInOwner->GetLocationAtSplinePoint(lIndexEnd, ESplineCoordinateSpace::World);

			lLengthTo = FVector::Dist(lStart, lEnd);
			lNumDots = FMath::RoundToInt32(lLengthTo / m_distanceBetweenPoints) + 1;

			for (int32 j = 0; j < lNumDots; j++)
			{
				lRatio = (float)j / (float)(lNumDots - 1);
				lCurrentLoc = FMath::Lerp(lStart, lEnd, lRatio);
				lTrans = FTransform(FQuat::Identity, lCurrentLoc, m_meshInstanceScale);

				m_meshVisualization->AddInstance(lTrans);
				m_meshVisualization->SetCustomDataValue(lTotalIndex++, 0, lLengthTo);

				lCurrentDist += m_distanceBetweenPoints;
			}
		}

		m_meshVisualization->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}