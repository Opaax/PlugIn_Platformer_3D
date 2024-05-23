// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "SubSystem/PMCheckpointSubsytem.h"
#include "Settings/PlatformerMaker_Settings.h"
#include "Triggerable/PMCheckpointActor.h"
#include "Utils/DebugMacro.h"

void UPMCheckpointSubsytem::OnCheckpointTrigger(APMCheckpointActor* CheckpointTriggered)
{
	if (CheckpointTriggered && CheckpointTriggered != m_currentCheckPoint) {
		m_currentCheckPoint = CheckpointTriggered;
		m_currentCheckPoint->ValidateCheckpoint();

		OnCheckpointChanged.Broadcast(m_currentCheckPoint, m_currentCheckPoint->GetPlayerStart());
	}
}

APlayerStart* UPMCheckpointSubsytem::GetCurrentCheckPointPlayerStart() const
{
	if (m_currentCheckPoint) {
		return m_currentCheckPoint->GetPlayerStart();
	}

	return nullptr;
}

bool UPMCheckpointSubsytem::ShouldCreateSubsystem(UObject* Outer) const
{
	UPlatformerMaker_Settings* lPluginSetting = GetMutableDefault<UPlatformerMaker_Settings>();

	if (!lPluginSetting->SpawnCheckpointSubSystem) {
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}

void UPMCheckpointSubsytem::Deinitialize()
{
	if (FCheckpointStaticEvent::OnCheckpointTrigger().IsBoundToObject(this) && m_checkpointEventHandle.IsValid()) {
		FCheckpointStaticEvent::OnCheckpointTrigger().Remove(m_checkpointEventHandle);
	}

	Super::Deinitialize();
}

void UPMCheckpointSubsytem::OnWorldBeginPlay(UWorld& InWorld)
{
	if (!FCheckpointStaticEvent::OnCheckpointTrigger().IsBoundToObject(this)) {
		m_checkpointEventHandle = FCheckpointStaticEvent::OnCheckpointTrigger().AddUFunction(this, FName("OnCheckpointTrigger"));
	}
}