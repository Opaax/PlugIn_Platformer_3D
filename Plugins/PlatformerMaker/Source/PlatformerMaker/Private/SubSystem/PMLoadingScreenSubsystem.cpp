// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "SubSystem/PMLoadingScreenSubsystem.h"
#include "Settings/PlatformerMaker_Settings.h"
#include "Utils/DebugMacro.h"
#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PMLoadingScreenSubsystem)

UPMLoadingScreenSubsystem::UPMLoadingScreenSubsystem()
{
	m_loading = nullptr;
	m_baseZOrder = 100;
	bIsAddedManually = false;
}

void UPMLoadingScreenSubsystem::BeginLoading(const FString& MapName)
{
	if (bIsAddedManually) {
		return;
	}

	AddLoadingOnScreen_Internal(m_baseZOrder, NULL);
}

void UPMLoadingScreenSubsystem::EndLoading(UWorld* InLoadedWorld)
{
	if (bIsAddedManually) {
		return;
	}

	RemoveLoadingFromScreen();
}

void UPMLoadingScreenSubsystem::AddLoadingOnScreen(int32 ZOrder, UUserWidget* overrideWidget)
{
	bIsAddedManually = true;
	AddLoadingOnScreen_Internal(ZOrder, overrideWidget);
}

void UPMLoadingScreenSubsystem::AddLoadingOnScreen_Internal(int32 ZOrder, UUserWidget* overrideWidget)
{
	if (IsValid(overrideWidget)) {
		m_loading = overrideWidget;
	}
	else{
		UPlatformerMaker_Settings* lPluginSetting = GetMutableDefault<UPlatformerMaker_Settings>();

		if (lPluginSetting->BaseLoadingWidget) {
			m_loading = NewObject<UUserWidget>(this, lPluginSetting->BaseLoadingWidget);
		}
	}

	if (!m_loading) {
		DEBUG_WARNING(TEXT("[%s], Trying to add loading on screen with NULL loading --- %s ---"), *GetFName().ToString(), *CURRENT_FUNC);
		return;
	}

	GEngine->GameViewport->AddViewportWidgetContent(m_loading->TakeWidget(), ZOrder);
}

void UPMLoadingScreenSubsystem::RemoveLoadingFromScreen_Internal()
{
	if (!m_loading) {
		return;
	}

	GEngine->GameViewport->RemoveViewportWidgetContent(m_loading->TakeWidget());
}

void UPMLoadingScreenSubsystem::RemoveLoadingFromScreen()
{
	bIsAddedManually = false;
	RemoveLoadingFromScreen_Internal();
}

bool UPMLoadingScreenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	UPlatformerMaker_Settings* lPluginSetting = GetMutableDefault<UPlatformerMaker_Settings>();

	if (!lPluginSetting->CreateLoadingSubsystem) {
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}

void UPMLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Show simple loading screen when transitioning between maps
	if (!IsRunningDedicatedServer()) {
		FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UPMLoadingScreenSubsystem::BeginLoading);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UPMLoadingScreenSubsystem::EndLoading);
	}
}