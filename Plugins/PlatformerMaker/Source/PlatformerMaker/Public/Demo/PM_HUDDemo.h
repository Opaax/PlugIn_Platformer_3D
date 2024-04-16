// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Demo/PM_DemoCoreDelegates.h"
#include "PM_HUDDemo.generated.h"

class UUserWidget;
class UPM_DeathTransitionDemo;
class UPMLifeComponent;

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

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter = "GetDefaultWidget", Category = "Runtime")
	TObjectPtr<UUserWidget> m_defaultWidget;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	TObjectPtr<UPM_DeathTransitionDemo> m_deathTransitionWidget;

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter = "GetCurrentLifeComp", Category = "Runtime")
	TObjectPtr<UPMLifeComponent> m_currentLifeComp;

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
	void LaunchDeathTransition(const float AnimSpeed, const FDeathTransitionCallback& FinishCallback);

	virtual void SetLifeComponent(const UPMLifeComponent* InLifeComp);

	void OnSetLifeComponent(const UPMLifeComponent* InLifeComp, bool HasChange);
	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "OnSetLifeComponent"))
	void ReceiveOnSetLifeComponent(const UPMLifeComponent* InLifeComp, bool HasChange);

	UFUNCTION(BlueprintPure)
	FORCEINLINE UUserWidget* GetDefaultWidget() const { return m_defaultWidget; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UPMLifeComponent* GetCurrentLifeComp() const { return m_currentLifeComp; }

	/*---------------------------------- OVERRIDE ----------------------------------*/
protected:
	void BeginPlay() override;
};
