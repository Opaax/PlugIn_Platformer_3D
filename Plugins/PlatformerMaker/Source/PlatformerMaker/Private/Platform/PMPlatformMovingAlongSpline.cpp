// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Platform/PMPlatformMovingAlongSpline.h"
#include "Tools/PMSplineActor.h"
#include "Components/PMInterpSplineFollowMovement.h"
#include "Utils/DebugMacro.h"

//Unreal
#include "Components/SplineComponent.h"

APMPlatformMovingAlongSpline::APMPlatformMovingAlongSpline(const FObjectInitializer& ObjInit):Super(ObjInit)
{
	SetRootComponent(m_meshPlatform);

	m_interpSplineFollowComp = CreateDefaultSubobject<UPMInterpSplineFollowMovement>(TEXT("InterpSplineFollowComponent"));
}

void APMPlatformMovingAlongSpline::BeginPlay()
{
	Super::BeginPlay();
}

void APMPlatformMovingAlongSpline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if (m_splineActor != nullptr) {
		BindSplineActorEditMove();
	}
#endif //WITH_EDITOR
}

#if WITH_EDITOR
void APMPlatformMovingAlongSpline::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DEBUG_ERROR(TEXT("edit: %s"), *PropertyChangedEvent.MemberProperty->NamePrivate.ToString());

	if (PropertyChangedEvent.MemberProperty->NamePrivate == "m_splineActor") {
		if (m_splineActor != nullptr && m_interpSplineFollowComp->GetSpline() == nullptr) {

			m_interpSplineFollowComp->SetSpline(m_splineActor->GetSplineComponent());

			ReplaceOnSplineEdit();
			BindSplineActorEditMove();
		}
		else if (m_splineActor == nullptr) {
			UnbindSplineActorEditMove();

			/*
			* Reset linked spline. 
			* It could be only a reference reset which don't mean that the spline is destroy. So the reference could be unsafe
			*/
			m_interpSplineFollowComp->SetSpline(nullptr);
		}
		else {
			ReplaceOnSplineEdit();
		}
	}
}

void APMPlatformMovingAlongSpline::PostEditUndo()
{
	Super::PostEditUndo();
}

void APMPlatformMovingAlongSpline::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	if (bFinished) {
		ReplaceOnSplineEdit();
	}
}

void APMPlatformMovingAlongSpline::PreEditUndo()
{
	Super::PreEditUndo();
}

void APMPlatformMovingAlongSpline::ReplaceOnSplineEdit()
{
	if (m_interpSplineFollowComp->GetSpline() != nullptr) {
		SetActorLocation(m_interpSplineFollowComp->GetSpline()->GetLocationAtSplineInputKey(0, ESplineCoordinateSpace::World));
	}
}

void APMPlatformMovingAlongSpline::BindSplineActorEditMove()
{
	if (!GEngine->OnActorMoved().IsBoundToObject(this)) {
		m_editSplineActorMoveHandle = GEngine->OnActorMoved().AddUObject(this, &APMPlatformMovingAlongSpline::OnSplineActorMoved);
	}
}

void APMPlatformMovingAlongSpline::UnbindSplineActorEditMove()
{
	if (GEngine->OnActorMoved().IsBoundToObject(this)) {
		GEngine->OnActorMoved().Remove(m_editSplineActorMoveHandle);
	}
}

void APMPlatformMovingAlongSpline::OnSplineActorMoved(AActor* MovedActor)
{
	if (m_splineActor == nullptr || MovedActor == nullptr) {
		DEBUG_ERROR(TEXT("%s, Looks like the platform as its spline actor set to null ptr. Set it to consider replacing on spline actor"), *GetNameSafe(this));
		return;
	}

	if (MovedActor == m_splineActor) {
		ReplaceOnSplineEdit();
	}
}
#endif // WITH_EDITOR