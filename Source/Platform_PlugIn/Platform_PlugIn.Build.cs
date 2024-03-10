// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Platform_PlugIn : ModuleRules
{
	public Platform_PlugIn(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ "ModularGameplay", "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", 
			"EnhancedInput", "PlatformerMaker", "GameplayTags", "GameplayAbilities" });

		PrivateDependencyModuleNames.AddRange(new string[] {});
	}
}
