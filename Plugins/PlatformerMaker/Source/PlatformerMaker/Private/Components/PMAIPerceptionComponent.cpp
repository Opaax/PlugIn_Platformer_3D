// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Components/PMAIPerceptionComponent.h"

#include "Perception/AISenseConfig.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/KismetSystemLibrary.h"

void UPMAIPerceptionComponent::OnRegister()
{
	Super::OnRegister();
}

void UPMAIPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UPMAIPerceptionComponent::GetSightAngle() const
{
	if (GetOwner())
	{
		if (GetOwner()->GetWorld())
		{
			for (auto SenseConfig : SensesConfig)
			{
				if (SenseConfig)
				{
					if (UAISenseConfig_Sight* lSigh = Cast<UAISenseConfig_Sight>(SenseConfig))
					{
						return lSigh->PeripheralVisionAngleDegrees;
					}
				}
			}
		}
	}

	return 0.0f;
}

float UPMAIPerceptionComponent::GetSightRadius() const
{
	if (GetOwner())
	{
		if (GetOwner()->GetWorld())
		{
			for (auto SenseConfig : SensesConfig)
			{
				if (SenseConfig)
				{
					if (UAISenseConfig_Sight* lSigh = Cast<UAISenseConfig_Sight>(SenseConfig))
					{
						return lSigh->SightRadius;
					}
				}
			}
		}
	}

	return 0.0f;
}
