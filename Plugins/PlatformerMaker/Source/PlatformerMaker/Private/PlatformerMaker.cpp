// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "PlatformerMaker.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Settings/PlatformerMaker_Settings.h"

#define LOCTEXT_NAMESPACE "FPlatformerMakerModule"

DEFINE_LOG_CATEGORY(LogPlatformerPlugin);

void FPlatformerMakerModule::StartupModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "PlatformerMaker_Settings",
			LOCTEXT("RuntimeSettingsName", "Platformer Maker"), LOCTEXT("RuntimeSettingsDescription", "You can set here some value to "),
			GetMutableDefault<UPlatformerMaker_Settings>());
	}
}

void FPlatformerMakerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "PlatformerMaker_Settings");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPlatformerMakerModule, PlatformerMaker)