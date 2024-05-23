// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Triggerable/PMCheckpointActor.h"
#include "Utils/DebugMacro.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Components/ChildActorComponent.h"

FCheckpointStaticEvent::FCheckpointTrigger FCheckpointStaticEvent::CheckpointTrigger;

APMCheckpointActor::APMCheckpointActor(const FObjectInitializer& ObjectInit) :Super(ObjectInit)
{
	// we need to do that
	//Simple because Player start is static
	//if the root and the Child actor is not statics that will return a warning and the child actor will be attached to the root
	GetRootComponent()->SetMobility(EComponentMobility::Static);

	m_childActorPlayerStart = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActorPlayerStart"));
	m_childActorPlayerStart->SetupAttachment(GetRootComponent());
	m_childActorPlayerStart->SetMobility(EComponentMobility::Static);
	m_childActorPlayerStart->SetChildActorClass(APlayerStart::StaticClass());

	TArray<USceneComponent*> lSceneComp;
	GetComponents<USceneComponent>(lSceneComp, true);

	//Make all static if scene comp
	for (auto* lComp : lSceneComp) {
		if (lComp) {
			lComp->SetMobility(EComponentMobility::Static);
		}
	}

	CheckChildSetPlayerStart();
}

void APMCheckpointActor::ValidateCheckpoint_Implementation()
{
	//TODO
	//Effect
	//will mainly bu used in BP
}

void APMCheckpointActor::CheckChildSetPlayerStart()
{
	if (m_playerStart != nullptr) {
		return;
	}

	if (APlayerStart* lPlayerStart = CastChecked<APlayerStart>(m_childActorPlayerStart->GetChildActor(), ECastCheckedType::NullAllowed)) {
		SetPlayerStart(lPlayerStart);
	}
}

void APMCheckpointActor::BeginPlay()
{
	Super::BeginPlay();

	CheckChildSetPlayerStart();
}

void APMCheckpointActor::OnTrigger(AActor* OtherTrigger)
{
	FCheckpointStaticEvent::OnCheckpointTrigger().Broadcast(this);

	Super::OnTrigger(OtherTrigger);
}
