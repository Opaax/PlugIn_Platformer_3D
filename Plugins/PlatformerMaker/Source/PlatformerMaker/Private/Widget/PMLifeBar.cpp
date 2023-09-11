// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Widget/PMLifeBar.h"
#include "Widget/PMDamageableBar.h"

void UPMLifeBar::InitializeValues(const float MaxLife, const float CurrentLife)
{
	m_maxLife = MaxLife;
	m_currentLife = CurrentLife;

	const float lRatio = m_currentLife / m_maxLife;

	if (LifeBar != nullptr)
	{
		LifeBar->SetInitialProgressValues(lRatio, 1.f);
	}
}

void UPMLifeBar::UpdateLife(const float NextLife, const float LifeRatio)
{
	m_currentLife = NextLife;

	if (LifeBar != nullptr)
	{
		LifeBar->SetTopProgress(LifeRatio);
	}
}
