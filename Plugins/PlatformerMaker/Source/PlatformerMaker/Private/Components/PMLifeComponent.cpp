// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Components/PMLifeComponent.h"
#include "PlatformerMaker.h"

UPMLifeComponent::UPMLifeComponent(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMLifeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPMLifeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPMLifeComponent::AddLife_Implementation(float Amount)
{
	m_currentLife = FMath::Clamp(m_currentLife + Amount, 0, m_maxLife);

	CheckLife();

	UE_LOG(LogPlatformerPlugin, Log, TEXT("%s: Add %f to the Current Life: %f"), *GetName(), Amount, m_currentLife);
}

void UPMLifeComponent::RemoveLife_Implementation(float Amount)
{
	m_currentLife = FMath::Clamp(m_currentLife - Amount, 0, m_maxLife);

	CheckLife();

	UE_LOG(LogPlatformerPlugin, Log, TEXT("%s: Remove %f to the Current Life: %f"), *GetName(), Amount, m_currentLife);
}

void UPMLifeComponent::CheckLife_Implementation()
{
	OnLifeUpdated.Broadcast(m_currentLife);

	if (m_currentLife <= 0)
	{
		OnNoMoreLife.Broadcast();

		UE_LOG(LogPlatformerPlugin, Log, TEXT("%s From %s: No more life"), *GetName(), *GetOwner()->GetName());
	}
}