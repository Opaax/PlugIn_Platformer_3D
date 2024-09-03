// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Platform_PlugIn : ModuleRules
{
	public Platform_PlugIn(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ "ModularGameplay", "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", 
			"EnhancedInput", "PlatformerMaker", "GameplayTags", "GameplayTasks", "GameplayAbilities","CommonUI" });

		PrivateDependencyModuleNames.AddRange(new string[] { "CommonUI" });
	}
}
