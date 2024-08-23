// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "SubSystem/PMCheckpointSubsystem.h"
#include "Settings/PlatformerMaker_Settings.h"
#include "Triggerable/PMCheckpointActor.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

//CTOR
UPMCheckpointSubsystem::UPMCheckpointSubsystem()
{
	m_currentCheckPoint = nullptr;
}

void UPMCheckpointSubsystem::OnCheckpointTrigger(APMCheckpointActor* CheckpointTriggered)
{
	if (CheckpointTriggered && CheckpointTriggered != m_currentCheckPoint) {
		m_currentCheckPoint = CheckpointTriggered;
		m_currentCheckPoint->ValidateCheckpoint();

		OnCheckpointChanged.Broadcast(m_currentCheckPoint, m_currentCheckPoint->GetPlayerStart());
	}
}

APlayerStart* UPMCheckpointSubsystem::GetCurrentCheckPointPlayerStart()
{
	if (m_currentCheckPoint) {
		return m_currentCheckPoint->GetPlayerStart();
	}

	return nullptr;
}

bool UPMCheckpointSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	UPlatformerMaker_Settings* lPluginSetting = GetMutableDefault<UPlatformerMaker_Settings>();

	if (!lPluginSetting->SpawnCheckpointSubSystem) {
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}

void UPMCheckpointSubsystem::Deinitialize()
{
	if (FCheckpointStaticEvent::OnCheckpointTrigger().IsBoundToObject(this) && m_checkpointEventHandle.IsValid()) {
		FCheckpointStaticEvent::OnCheckpointTrigger().Remove(m_checkpointEventHandle);
	}

	Super::Deinitialize();
}

void UPMCheckpointSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	if (!FCheckpointStaticEvent::OnCheckpointTrigger().IsBoundToObject(this)) {
		m_checkpointEventHandle = FCheckpointStaticEvent::OnCheckpointTrigger().AddUFunction(this, FName("OnCheckpointTrigger"));
	}

	for (TActorIterator<APMCheckpointActor> It(GetWorld(), APMCheckpointActor::StaticClass()); It; ++It)
	{
		APMCheckpointActor* CheckPoint = *It;

		if (CheckPoint && CheckPoint->IsPlayerStart()) {
			m_currentCheckPoint = CheckPoint;
			return;
		}
	}
}