// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "SubSystem/PMLoadingScreenSubsystem.h"
#include "Settings/PlatformerMaker_Settings.h"
#include "Utils/DebugMacro.h"
#include "Widget/PMUW_LoadingBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PMLoadingScreenSubsystem)

UPMLoadingScreenSubsystem::UPMLoadingScreenSubsystem()
{
	m_loading = nullptr;
	m_baseZOrder = 100;
}

void UPMLoadingScreenSubsystem::PreMapLoading(const FString& MapName)
{
	OnPreMapLoad.Broadcast(MapName);

	AddLoadingOnScreen_Internal(m_baseZOrder, NULL);
}

void UPMLoadingScreenSubsystem::PostMapLoaded(UWorld* InLoadedWorld)
{
	OnPostMapLoaded.Broadcast(InLoadedWorld);

	RemoveLoadingFromScreen();
}

void UPMLoadingScreenSubsystem::AddLoadingOnScreen(int32 ZOrder, UPMUW_LoadingBase* overrideWidget)
{
	AddLoadingOnScreen_Internal(ZOrder, overrideWidget);
}

void UPMLoadingScreenSubsystem::AddLoadingOnScreen_Internal(int32 ZOrder, UPMUW_LoadingBase* overrideWidget)
{
	//Added manually
	if (m_loading) {
		return;
	}

	if (IsValid(overrideWidget)) {
		m_loading = overrideWidget;
	}
	else{
		UPlatformerMaker_Settings* lPluginSetting = GetMutableDefault<UPlatformerMaker_Settings>();

		if (lPluginSetting->BaseLoadingWidget) {
			m_loading = NewObject<UPMUW_LoadingBase>(this, lPluginSetting->BaseLoadingWidget);
		}
	}

	if (!m_loading) {
		DEBUG_WARNING(TEXT("[%s], Trying to add loading on screen with NULL loading --- %s ---"), *GetFName().ToString(), *CURRENT_FUNC);
		return;
	}

	GEngine->GameViewport->AddViewportWidgetContent(m_loading->TakeWidget(), ZOrder);

	m_loading->OnLoadingBeginAnimDone.Add(m_loadingScreenBeginAnimDelegate);
	m_loading->StartLoadAnim();
}

void UPMLoadingScreenSubsystem::RemoveLoadingFromScreen_Internal()
{
	if (!m_loading) {
		return;
	}

	m_loading->OnLoadingEndingAnimDone.Add(m_loadingScreenEndingAnimDelegate);
	m_loading->StopLoadAnim();
}

void UPMLoadingScreenSubsystem::OnLoadingScreenBeginAnimFinish()
{
	m_loading->OnLoadingBeginAnimDone.Remove(m_loadingScreenBeginAnimDelegate);
	OnLoadingScreenBeginFinish.Broadcast();
}

void UPMLoadingScreenSubsystem::OnLoadingScreenEndingAnimFinish()
{
	m_loading->OnLoadingEndingAnimDone.Remove(m_loadingScreenEndingAnimDelegate);
	OnLoadingScreenEndingFinish.Broadcast();
	GEngine->GameViewport->RemoveViewportWidgetContent(m_loading->TakeWidget());
}

void UPMLoadingScreenSubsystem::RemoveLoadingFromScreen()
{
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
		FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UPMLoadingScreenSubsystem::PreMapLoading);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UPMLoadingScreenSubsystem::PostMapLoaded);

		m_loadingScreenBeginAnimDelegate.BindUFunction(this, FName("OnLoadingScreenBeginAnimFinish"));
		m_loadingScreenEndingAnimDelegate.BindUFunction(this, FName("OnLoadingScreenEndingAnimFinish"));
	}
}