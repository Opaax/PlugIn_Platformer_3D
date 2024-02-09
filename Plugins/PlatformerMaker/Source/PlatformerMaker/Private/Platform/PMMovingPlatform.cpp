// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Platform/PMMovingPlatform.h"
#include "../../Public/Components/PMInterpToMovementComponent.h"

//Unreal
#include "Components/SplineComponent.h"

APMMovingPlatform::APMMovingPlatform(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	m_splineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	m_splineComponent->SetupAttachment(m_root);

	m_interpToMovementComponent = CreateDefaultSubobject<UPMInterpToMovementComponent>(TEXT("InterpComponent"));

	m_interpToMovementComponent->BehaviourType = EInterpToBehaviourType::PingPong;
}

void APMMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	SetInterpPoints();
}

void APMMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APMMovingPlatform::SetInterpPoints()
{
	if (IsValid(m_splineComponent) && IsValid(m_interpToMovementComponent))
	{
		int32 lNumPoints = m_splineComponent->GetNumberOfSplinePoints();

		FVector lPointLocation;

		//Find out all spline points and add it to interp comp
		for (int32 i = 0; i < lNumPoints; i++)
		{
			lPointLocation = m_splineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);

			m_interpToMovementComponent->AddControlPointPosition(lPointLocation);
		}
	}

	FinalizeInterpComp();
}

void APMMovingPlatform::FinalizeInterpComp()
{
	m_interpToMovementComponent->Duration = m_moveDuration;
	m_interpToMovementComponent->FinaliseControlPoints();
}
