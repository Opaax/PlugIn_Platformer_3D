// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PMLoadingScreenSubsystem.generated.h"

class UPMUW_LoadingBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadingScreenPreLoad, const FString&, MapName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadingScreenPostLoad, UWorld*, World);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadingScreenAnimSignature);

/**
 * Subsystem that Allow us to add permanent loading screen
 */
UCLASS()
class PLATFORMERMAKER_API UPMLoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
protected:
	UPROPERTY()
	TObjectPtr<UPMUW_LoadingBase> m_loading;

	UPROPERTY()
	int32 m_baseZOrder;

	FScriptDelegate m_loadingScreenBeginAnimDelegate;
	FScriptDelegate m_loadingScreenEndingAnimDelegate;

public:
	UPROPERTY(BlueprintAssignable)
	FLoadingScreenPreLoad OnPreMapLoad;
	
	UPROPERTY(BlueprintAssignable)
	FLoadingScreenPostLoad OnPostMapLoaded;

	UPROPERTY(BlueprintAssignable)
	FLoadingScreenAnimSignature OnLoadingScreenBeginFinish;

	UPROPERTY(BlueprintAssignable)
	FLoadingScreenAnimSignature OnLoadingScreenEndingFinish;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
protected:
	/* Auto called when a map is open*/
	UFUNCTION(BlueprintCallable, Category = "Platformer|Loading")
	void PreMapLoading(const FString& MapName);
	
	/* Auto called when a map is loaded*/
	UFUNCTION(BlueprintCallable, Category = "Platformer|Loading")
	void PostMapLoaded(UWorld* InLoadedWorld);

	/* */
	UFUNCTION(Category = "Platformer|Loading")
	void AddLoadingOnScreen_Internal(int32 ZOrder, UPMUW_LoadingBase* overrideWidget = nullptr);

	UFUNCTION(Category = "Platformer|Loading")
	void RemoveLoadingFromScreen_Internal();

	UFUNCTION(Category = "Platformer|Loading")
	void OnLoadingScreenBeginAnimFinish();

	UFUNCTION(Category = "Platformer|Loading")
	void OnLoadingScreenEndingAnimFinish();

public:
	UPMLoadingScreenSubsystem();

	/* This should be called before changing level */
	UFUNCTION(BlueprintCallable, Category = "Platformer|Loading")
	void AddLoadingOnScreen(int32 ZOrder, UPMUW_LoadingBase* overrideWidget = nullptr);

	/* Set Loading Screen widget */
	UFUNCTION(BlueprintCallable, Category = "Platformer|Loading")
	void RemoveLoadingFromScreen();

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
