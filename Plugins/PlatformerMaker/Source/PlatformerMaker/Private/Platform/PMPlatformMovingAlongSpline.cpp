// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Platform/PMPlatformMovingAlongSpline.h"

APMPlatformMovingAlongSpline::APMPlatformMovingAlongSpline(const FObjectInitializer& ObjInit):Super(ObjInit)
{
	SetRootComponent(m_meshPlatform);
}

void APMPlatformMovingAlongSpline::BeginPlay()
{
	Super::BeginPlay();
}

void APMPlatformMovingAlongSpline::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void APMPlatformMovingAlongSpline::PostEditUndo()
{
	Super::PostEditUndo();
}

void APMPlatformMovingAlongSpline::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
}

void APMPlatformMovingAlongSpline::PreEditUndo()
{
	Super::PreEditUndo();
}
