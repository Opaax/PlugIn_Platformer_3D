//2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Tools/PMSplineActor.h"

#include "Components/SplineComponent.h"

APMSplineActor::APMSplineActor(const FObjectInitializer& ObjInit):Super(ObjInit)
{
	PrimaryActorTick.bCanEverTick = true;

	m_splineComp = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void APMSplineActor::BeginPlay()
{
	Super::BeginPlay();

	CheckDestroyAtBeginPlay();
}

void APMSplineActor::CheckDestroyAtBeginPlay()
{
	if (bDestroyAtStart) {
		Destroy();
	}
}