// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Triggerable/PMCheckpointActor.h"
#include "Utils/DebugMacro.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

FCheckpointStaticEvent::FCheckpointTrigger FCheckpointStaticEvent::CheckpointTrigger;

APMCheckpointActor::APMCheckpointActor(const FObjectInitializer& ObjectInit) :Super(ObjectInit)
{
	m_playerStart = CreateDefaultSubobject<APlayerStart>(TEXT("RespawnPlayerStart"));
}

void APMCheckpointActor::ValidateCheckpoint_Implementation()
{
	//TODO
	//Effect?
}

void APMCheckpointActor::OnTrigger(AActor* OtherTrigger)
{
	FCheckpointStaticEvent::OnCheckpointTrigger().Broadcast(this);

	Super::OnTrigger(OtherTrigger);
}
