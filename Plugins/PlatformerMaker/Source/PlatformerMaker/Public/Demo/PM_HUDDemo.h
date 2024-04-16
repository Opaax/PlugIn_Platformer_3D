// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Demo/PM_DemoCoreDelegates.h"
#include "PM_HUDDemo.generated.h"

class UUserWidget;
class UPM_DeathTransitionDemo;

/**
 * Base HUD class for the demo.
 * 
 * Instantiate default widget for the demo
 */
UCLASS()
class PLATFORMERMAKER_API APM_HUDDemo : public AHUD
{
	GENERATED_UCLASS_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
private:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets", meta = (DisplayName = "DefaultWidgetClass"))
	TSubclassOf<UUserWidget> m_defaultWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets", meta = (DisplayName = "DeathTransitionWidgetClass"))
	TSubclassOf<UPM_DeathTransitionDemo> m_deathTransitionWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	TObjectPtr<UUserWidget> m_defaultWidget;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	TObjectPtr<UPM_DeathTransitionDemo> m_deathTransitionWidget;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
protected:
	UFUNCTION(Category = "DemoHUD")
	void CreateDefaultWidget();

	UFUNCTION(Category = "DemoHUD")
	void CreateDeathTranstionWidget();

	virtual void OnDefaultWidgetCreated(UUserWidget* CreatedWidget);
	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnDefaultWidgetCreated"))
	void ReceiveOnDefaultWidgetCreated(UUserWidget* CreatedWidget);

	virtual void OnDefaultWidgetAddedOnScreen(UUserWidget* AddedWidget);
	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnDefaultWidgetAddedOnScreen"))
	void ReceiveOnDefaultWidgetAddedOnScreen(UUserWidget* AddedWidget);

public:
	UFUNCTION(Category = "DemoHUD")
	void LaunchDeathTranstion(const float AnimSpeed, const FDeathTransitionCallback& FinishCallback);

	/*---------------------------------- OVERRIDE ----------------------------------*/
protected:
	void BeginPlay() override;
};
