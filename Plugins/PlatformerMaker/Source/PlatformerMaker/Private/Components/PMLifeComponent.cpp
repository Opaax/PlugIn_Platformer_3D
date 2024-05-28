// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Components/PMLifeComponent.h"
#include "PlatformerMaker.h"
#include "GameFramework/Actor.h"

//Unreal
#if WITH_EDITOR
#include "HAL/IConsoleManager.h"
#include "Misc/DefaultValueHelper.h"
#endif

UPMLifeComponent::UPMLifeComponent(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	bRegisterConsoleFunctions = true;
#endif
}

void UPMLifeComponent::BeginPlay()
{
	Super::BeginPlay();

	m_currentLife = GetStartLife();
}

void UPMLifeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPMLifeComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	RegisterCMDFunctions();
#endif
}

void UPMLifeComponent::OnUnregister()
{
	Super::OnUnregister();

#if WITH_EDITOR
	UnregisterCMDFunctions();
#endif
}

#if WITH_EDITOR
void UPMLifeComponent::RegisterCMDFunctions()
{
	if (GetOwner() && bRegisterConsoleFunctions)
	{
		if (GetOwner()->GetWorld())
		{
			if (GetOwner()->GetWorld()->IsPlayInEditor())
			{
				FString lCMDRemoveName = FString::Printf(TEXT("PMRemoveLife_%s"), *GetNameSafe(GetOwner()));
				FString lCMDAddName = FString::Printf(TEXT("PMAddLife_%s"), *GetNameSafe(GetOwner()));
				FString lCMDRemoveHelp = FString::Printf(TEXT("Remove life in %s"), *GetNameSafe(GetOwner()));
				FString lCMDAddHelp = FString::Printf(TEXT("Add life in %s"), *GetNameSafe(GetOwner()));

				const TCHAR* lRemoveName = *lCMDRemoveName;
				const TCHAR* lAddName = *lCMDAddName;
				const TCHAR* lRemoveHelp = *lCMDRemoveHelp;
				const TCHAR* lAddHelp = *lCMDAddHelp;

				m_cmdFunctions.Add(IConsoleManager::Get().RegisterConsoleCommand(lRemoveName,
					lRemoveHelp,
					FConsoleCommandWithArgsDelegate::CreateUObject(this, &UPMLifeComponent::DebugRemoveLife),
					ECVF_Default));

				m_cmdFunctions.Add(IConsoleManager::Get().RegisterConsoleCommand(lAddName,
					lAddHelp,
					FConsoleCommandWithArgsDelegate::CreateUObject(this, &UPMLifeComponent::DebugAddLife),
					ECVF_Default));
			}
		}
	}
}

void UPMLifeComponent::UnregisterCMDFunctions()
{
	if (m_cmdFunctions.Num() < 1 || !bRegisterConsoleFunctions)
	{
		return;
	}

	for (auto& CmdConsole : m_cmdFunctions)
	{
		if (CmdConsole != nullptr)
		{
			IConsoleManager::Get().UnregisterConsoleObject(CmdConsole);
		}
	}
}

void UPMLifeComponent::DebugRemoveLife(const TArray<FString>& Args) 
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Remove life, Not enough arguments"));
	}
	else if (Args.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Remove Life, Too much arguments"));
	}

	if (GetWorld())
	{
		if (Args[0].IsNumeric())
		{
			int32 lRemove;
			FDefaultValueHelper::ParseInt(Args[0], lRemove);

			RemoveLife(lRemove);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Remove Life, Can't parse the arguments as int"));
		}
	}
}
void UPMLifeComponent::DebugAddLife(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Add life, Not enough arguments"));
	}
	else if (Args.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Add Life, Too much arguments"));
	}

	if (GetWorld())
	{
		if (Args[0].IsNumeric())
		{
			int32 lAdd;
			FDefaultValueHelper::ParseInt(Args[0], lAdd);

			AddLife(lAdd);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Add Life, Can't parse the arguments as int"));
		}
	}
}
#endif

void UPMLifeComponent::AddLife_Implementation(float Amount)
{
	m_currentLife = FMath::Clamp(m_currentLife + Amount, 0, m_maxLife);

	CheckLife();

	UE_LOG(LogPlatformerPlugin, Log, TEXT("%s: Add %f to the Current Life: %f in %s"), *GetName(), Amount, m_currentLife, *GetOwner()->GetName());
}

void UPMLifeComponent::RemoveLife_Implementation(float Amount)
{
	m_currentLife = FMath::Clamp(m_currentLife - Amount, 0, m_maxLife);

	CheckLife();

	UE_LOG(LogPlatformerPlugin, Log, TEXT("%s: Remove %f to the Current Life: %f in %s"), *GetName(), Amount, m_currentLife, *GetOwner()->GetName());
}

void UPMLifeComponent::CheckLife_Implementation()
{
	OnLifeUpdated.Broadcast(m_currentLife);

	if (m_currentLife <= 0)
	{
		OnNoMoreLife.Broadcast();

		UE_LOG(LogPlatformerPlugin, Log, TEXT("%s From %s: No more life"), *GetName(), *GetOwner()->GetName());
	}
}