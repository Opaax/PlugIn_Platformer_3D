// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "../Public/Platform/PMPlatform.h"
#include "PlatformerMaker.h"

//Unreal
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

APMPlatform::APMPlatform(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(m_root);

	m_meshPlatform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshPlatform"));
	m_meshPlatform->SetupAttachment(m_root);
}

void APMPlatform::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APMPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

