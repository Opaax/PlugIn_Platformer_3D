// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Triggarable/PMCoin.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APMCoin::APMCoin(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<USphereComponent>(TriggerComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	m_coinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	m_coinMesh->SetupAttachment(m_root);
}

void APMCoin::BeginPlay()
{
	Super::BeginPlay();	
}

void APMCoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

