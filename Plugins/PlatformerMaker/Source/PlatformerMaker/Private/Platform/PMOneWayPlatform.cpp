// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Platform/PMOneWayPlatform.h"

#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Utils/DebugMacro.h"

#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif

APMOneWayPlatform::APMOneWayPlatform(const FObjectInitializer& ObjectInit) :Super(ObjectInit)
{
	m_triggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	m_triggerComponent->SetupAttachment(m_root);

	m_oneWayDirection = CreateDefaultSubobject<USceneComponent>(TEXT("OneWayDirection"));
	m_oneWayDirection->SetupAttachment(m_root);
	m_oneWayDirection->SetRelativeRotation(FQuat(FVector(0,1,0), 90.f));

#if WITH_EDITORONLY_DATA
	m_arrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	m_arrowComponent->SetupAttachment(m_oneWayDirection);
#endif

	m_validClassToTrigger.Add(ACharacter::StaticClass());
}

void APMOneWayPlatform::OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && m_validClassToTrigger.Contains(OtherActor->GetClass())) {

		m_currentTriggerActor = OtherActor;
		m_currentActorRootPrimitive = CastChecked<UPrimitiveComponent>(m_currentTriggerActor->GetRootComponent(), ECastCheckedType::NullAllowed);

		ComputeOneWayCollision();
		OnTrigger(OtherActor);
	}
}

void APMOneWayPlatform::OnTriggerComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor) && m_validClassToTrigger.Contains(OtherActor->GetClass())) {
		m_currentTriggerActor = nullptr;
		m_currentActorRootPrimitive = nullptr;
		OnOutTrigger(OtherActor);
	}
}

void APMOneWayPlatform::OnTrigger(AActor* OtherTrigger)
{
	ReceiveOnTrigger(OtherTrigger);
}

void APMOneWayPlatform::OnOutTrigger(AActor* OtherTrigger)
{
	ReceiveOnOutTrigger(OtherTrigger);
}

void APMOneWayPlatform::ComputeOneWayCollision_Implementation()
{
	if (!IsValid(m_currentTriggerActor)) {
		return;
	}

	FVector lToActor = (m_currentTriggerActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	float lDot = m_oneWayDirection->GetForwardVector().Dot(lToActor);
	
	if (lDot < 0 || (m_currentTriggerActor->IsOverlappingActor(this) && lDot < m_overlappingDotComputation)) {
		if (m_currentActorRootPrimitive) {
			m_currentActorRootPrimitive->IgnoreComponentWhenMoving(m_meshPlatform, true);
		}
	} else {
		if(m_currentActorRootPrimitive) {
			m_currentActorRootPrimitive->IgnoreComponentWhenMoving(m_meshPlatform, false);
		}
	}
}

void APMOneWayPlatform::BeginPlay()
{
	Super::BeginPlay();

	m_triggerComponent->OnComponentBeginOverlap.AddDynamic(this, &APMOneWayPlatform::OnTriggerComponentOverlapped);
	m_triggerComponent->OnComponentEndOverlap.AddDynamic(this, &APMOneWayPlatform::OnTriggerComponentEndOverlapped);
}

void APMOneWayPlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsValid(m_currentTriggerActor)) {
		ComputeOneWayCollision();
	}
}