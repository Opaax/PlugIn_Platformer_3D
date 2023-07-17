// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Components/PMAIPerceptionComponent.h"

#include "Perception/AISenseConfig.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/KismetSystemLibrary.h"

void UPMAIPerceptionComponent::OnRegister()
{
	Super::OnRegister();

	for (auto SenseConfig : SensesConfig)
	{
		if (SenseConfig)
		{
			if (UAISenseConfig_Sight* lSigh = Cast<UAISenseConfig_Sight>(SenseConfig))
			{
				UE_LOG(LogTemp, Error, TEXT("%f"), lSigh->SightRadius);

				if (GetOwner() && GetOwner()->GetWorld())
				{

					//UKismetSystemLibrary::FlushPersistentDebugLines(GetOwner());
					//UKismetSystemLibrary::DrawDebugCone(GetOwner()->GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorForwardVector(), lSigh->SightRadius, FMath::Cos(lSigh->PeripheralVisionAngleDegrees), FMath::Cos(lSigh->PeripheralVisionAngleDegrees), 12, FColor::Red, 1000.f, 3.f);
				}
			}
		}
	}
}
