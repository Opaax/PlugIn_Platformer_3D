// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/PM_HUDDemo.h"
#include "Blueprint/UserWidget.h"
#include "Demo/PM_PlayerControllerDemo.h"
#include "Demo/UI/PM_DeathTransitionDemo.h"

APM_HUDDemo::APM_HUDDemo(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	m_defaultWidgetClass = UUserWidget::StaticClass();
}

void APM_HUDDemo::CreateDefaultWidget()
{
	if (!m_defaultWidgetClass || !GetOwningPlayerController()) {
		return;
	}

	m_defaultWidget = CreateWidget(GetOwningPlayerController(), m_defaultWidgetClass, FName("DefaultWidgetHUD"));

	if (!m_defaultWidget) {
		return;
	}

	OnDefaultWidgetCreated(m_defaultWidget);

	if (m_defaultWidget->AddToPlayerScreen(0)) {
		OnDefaultWidgetAddedOnScreen(m_defaultWidget);
	}
}

void APM_HUDDemo::CreateDeathTranstionWidget()
{
	if (!m_defaultWidgetClass || !GetOwningPlayerController())
	{
		return;
	}

	if(GetOwningPlayerController())
	{
		if(APM_PlayerControllerDemo* lPMDemoCtr = Cast<APM_PlayerControllerDemo>(GetOwningPlayerController()))
		{
			//Do not create Death transition if we are going back to menu.
			if(lPMDemoCtr->IsReturningToMenu())
			{
				return;
			}
		}
	}

	m_deathTransitionWidget = CreateWidget<UPM_DeathTransitionDemo>(GetOwningPlayerController(), m_deathTransitionWidgetClass, FName("TransitionWidgetHUD"));
}

void APM_HUDDemo::OnDefaultWidgetCreated(UUserWidget* CreatedWidget)
{
	ReceiveOnDefaultWidgetCreated(CreatedWidget);
}

void APM_HUDDemo::OnDefaultWidgetAddedOnScreen(UUserWidget* AddedWidget)
{
	ReceiveOnDefaultWidgetAddedOnScreen(AddedWidget);
}

void APM_HUDDemo::LaunchDeathTransition(const float AnimSpeed, const FDeathTransitionCallback& FinishCallback)
{
	if (!m_deathTransitionWidget) {
		CreateDeathTranstionWidget();

		if (!m_deathTransitionWidget) {
			FinishCallback.ExecuteIfBound();
			return;
		}
	}

	float lRealSpeed = FMath::Clamp(AnimSpeed, 1.f, 5.f);

	m_deathTransitionWidget->AddToPlayerScreen(10);
	m_deathTransitionWidget->StartTransition(lRealSpeed, FinishCallback);
}

void APM_HUDDemo::SetLifeComponent(const UPMLifeComponent* InLifeComp)
{
	if (m_currentLifeComp != nullptr && m_currentLifeComp == InLifeComp) {
		OnSetLifeComponent(m_currentLifeComp, false);
		return;
	}

	m_currentLifeComp = const_cast<UPMLifeComponent*>(InLifeComp);

	OnSetLifeComponent(m_currentLifeComp, true);
}

void APM_HUDDemo::OnSetLifeComponent(const UPMLifeComponent* InLifeComp, bool HasChange)
{
	ReceiveOnSetLifeComponent(InLifeComp, HasChange);
}

void APM_HUDDemo::BeginPlay()
{
	Super::BeginPlay();

	CreateDefaultWidget();
}
