// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Triggerable/PMCoin.h"

//Unreal
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

FName APMCoin::RotatingComponentName(TEXT("RotatingComponent"));

void APMCoin::OnCoinTrigger(int32 Coinvalue, AActor* TriggerBy)
{
	OnCoinTriggerDelegate.Broadcast(Coinvalue, TriggerBy);

	ReceiveOnCoinTrigger(Coinvalue, TriggerBy);
}

APMCoin::APMCoin(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<USphereComponent>(TriggerComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	m_coinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	m_coinMesh->SetupAttachment(m_root);

	m_rotatingComponent = CreateOptionalDefaultSubobject<URotatingMovementComponent>(RotatingComponentName);
}

void APMCoin::BeginPlay()
{
	Super::BeginPlay();	
}

void APMCoin::OnTrigger(AActor* OtherTrigger)
{
	Super::OnTrigger(OtherTrigger);

	OnCoinTrigger(m_coinValue,OtherTrigger);
}

void APMCoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

