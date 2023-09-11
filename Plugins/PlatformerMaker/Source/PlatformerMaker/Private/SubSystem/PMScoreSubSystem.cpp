// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "SubSystem/PMScoreSubSystem.h"
#include "Settings/PlatformerMaker_Settings.h"

bool UPMScoreSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	UPlatformerMaker_Settings* lPluginSetting = GetMutableDefault<UPlatformerMaker_Settings>();
	
	if (!lPluginSetting->SpawnScoreSubSystem) {
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}