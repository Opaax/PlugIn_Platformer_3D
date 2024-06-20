// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Platform/PMTimePlatform.h"
#include "Utils/DebugMacro.h"

#include "Components/BoxComponent.h"

APMTimePlatform::APMTimePlatform(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	m_triggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	m_triggerComponent->SetupAttachment(m_root);

	m_timeToDisable = 2.0f;
	m_elapsedTime	= 0.0f;
	m_timeToReset	= 0.4f;
	bIsTriggered	= false;
}

void APMTimePlatform::OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && m_validClassToTrigger.Contains(OtherActor->GetClass())) {
		OnTrigger(OtherActor);
	}
}

void APMTimePlatform::OnTrigger(AActor* OtherTrigger)
{
	if (bIsTriggered) {
		return;
	}

	bIsTriggered = true;

	m_elapsedTime = 0;
	GetWorld()->GetTimerManager().SetTimer(m_timeHandle, this, &APMTimePlatform::OnTimerUpdate, GetWorld()->DeltaTimeSeconds, true);

	ReceiveOnTrigger(OtherTrigger);
	OnTimerStart();
}

void APMTimePlatform::OnTimerStart()
{
	ReceiveOnTimerStart();
}

void APMTimePlatform::OnCollisionDisable()
{
	ReceiveOnCollisionDisable();
}

void APMTimePlatform::OnCollisionReset()
{
	bIsTriggered = false;
	m_timeHandle.Invalidate();
	m_meshPlatform->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ReceiveOnCollisionReset();
	
	//IT could happen that the player jump right on the frame that do not take in account the overlap.
	// Check overlap list and check if any of valid class is inside
	//Retrigger if true
	TArray<AActor*> lOutActors;

	for (int i = m_validClassToTrigger.Num() - 1; i >= 0; i--)
	{
		GetOverlappingActors(lOutActors, m_validClassToTrigger[i]);

		if (!lOutActors.IsEmpty()) {
			OnTrigger(lOutActors[0]);
			break;
		}
	}
}

void APMTimePlatform::OnTimerUpdated(float ElapsedTime)
{
	ReceiveOnTimerUpdated(ElapsedTime);
}

void APMTimePlatform::OnTimerUpdate()
{
	if (m_timeHandle.IsValid()) {
		m_elapsedTime += GetWorld()->DeltaTimeSeconds;

		OnTimerUpdated(m_elapsedTime);

		if (m_elapsedTime >= m_timeToDisable) {
			DisableCollision();
			StartResetTimer();
		}
	}
}

void APMTimePlatform::DisableCollision()
{
	m_meshPlatform->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnCollisionDisable();
}

void APMTimePlatform::StartResetTimer()
{
	m_elapsedTime = 0;
	GetWorld()->GetTimerManager().SetTimer(m_timeHandle, this, &APMTimePlatform::OnCollisionReset, m_timeToReset, false);
}

void APMTimePlatform::BeginPlay()
{
	Super::BeginPlay();

	m_triggerComponent->OnComponentBeginOverlap.AddDynamic(this, &APMTimePlatform::OnTriggerComponentOverlapped);
}
