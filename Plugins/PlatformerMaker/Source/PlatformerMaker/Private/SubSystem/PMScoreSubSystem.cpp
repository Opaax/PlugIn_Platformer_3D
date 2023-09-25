// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "SubSystem/PMScoreSubSystem.h"
#include "Settings/PlatformerMaker_Settings.h"

//Unreal
#if WITH_EDITOR
#include "HAL/IConsoleManager.h"
#include "Misc/DefaultValueHelper.h"
#endif

void UPMScoreSubSystem::AddScore_Implementation(int32 AddScore)
{
	m_currentScore += AddScore;

	UE_LOG(LogTemp, Warning, TEXT("Current Score %d"), m_currentScore);

	OnScoreUpdated.Broadcast(m_currentScore);
}

void UPMScoreSubSystem::RemoveScore_Implementation(int32 RemoveScore)
{
	m_currentScore = FMath::Clamp(m_currentScore -= RemoveScore, 0, TNumericLimits<int32>::Max());

	OnScoreUpdated.Broadcast(m_currentScore);
}

bool UPMScoreSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	UPlatformerMaker_Settings* lPluginSetting = GetMutableDefault<UPlatformerMaker_Settings>();
	
	if (!lPluginSetting->SpawnScoreSubSystem) {
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}

void UPMScoreSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//Register some cmd
#if WITH_EDITOR
	m_cmdSubScore.Add(IConsoleManager::Get().RegisterConsoleCommand(TEXT("PMAddScore"),
		TEXT("This a command to Add Score"),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UPMScoreSubSystem::DebugAddScore),
		ECVF_Default));

	m_cmdSubScore.Add(IConsoleManager::Get().RegisterConsoleCommand(TEXT("PMRemoveScore"),
		TEXT("This a command to Remove Score"),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UPMScoreSubSystem::DebugRemoveScore),
		ECVF_Default));
#endif
}

void UPMScoreSubSystem::Deinitialize()
{
	Super::Deinitialize();

	//Unregister some cmd
#if WITH_EDITOR
	for (auto& CmdConsole : m_cmdSubScore)
	{
		if (CmdConsole != nullptr)
		{
			IConsoleManager::Get().UnregisterConsoleObject(CmdConsole);
		}
	}
#endif
}

//Debug function to be register
#if WITH_EDITOR
void UPMScoreSubSystem::DebugRemoveScore(const TArray<FString>& Args) 
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Remove Score, Not enough arguments"));
	}
	else if (Args.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Remove Score, Too much arguments"));
	}

	if (GetWorld())
	{
		if (Args[0].IsNumeric())
		{
			int32 lRemove;
			FDefaultValueHelper::ParseInt(Args[0], lRemove);

			RemoveScore(lRemove);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Remove Score, Can't parse the arguments as int"));
		}
	}
}

void UPMScoreSubSystem::DebugAddScore(const TArray<FString>& Args)
{
	if (Args.Num() < 1) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Add Score, Not enough arguments"));
	}
	else if (Args.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Add Score, Too much arguments"));
	}

	if (GetWorld()) 
	{
		if (Args[0].IsNumeric()) 
		{
			int32 lAdded;
			FDefaultValueHelper::ParseInt(Args[0], lAdded);

			AddScore(lAdded);
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Add Score, Can't parse the arguments as int"));
		}
	}
}
#endif