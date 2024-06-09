// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PMLoadingScreenSubsystem.generated.h"

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
	TObjectPtr<UUserWidget> m_loading;

	UPROPERTY()
	int32 m_baseZOrder;

	UPROPERTY()
	bool bIsAddedManually;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
protected:
	/* Auto called when a map is open*/
	UFUNCTION(BlueprintCallable, Category = "Platformer|Loading")
	void BeginLoading(const FString& MapName);
	
	/* Auto called when a map is loaded*/
	UFUNCTION(BlueprintCallable, Category = "Platformer|Loading")
	void EndLoading(UWorld* InLoadedWorld);

	/* */
	UFUNCTION(Category = "Platformer|Loading")
	void AddLoadingOnScreen_Internal(int32 ZOrder, UUserWidget* overrideWidget = nullptr);

	UFUNCTION(Category = "Platformer|Loading")
	void RemoveLoadingFromScreen_Internal();

public:
	UPMLoadingScreenSubsystem();

	/* This should be called before changing level */
	UFUNCTION(BlueprintCallable, Category = "Platformer|Loading")
	void AddLoadingOnScreen(int32 ZOrder, UUserWidget* overrideWidget = nullptr);

	/* Set Loading Screen widget */
	UFUNCTION(BlueprintCallable, Category = "Platformer|Loading")
	void RemoveLoadingFromScreen();

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
