// Copyright Epic Games, Inc. All Rights Reserved.

#include "Platform_PlugInGameMode.h"
#include "Platform_PlugInCharacter.h"
#include "UObject/ConstructorHelpers.h"

APlatform_PlugInGameMode::APlatform_PlugInGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
