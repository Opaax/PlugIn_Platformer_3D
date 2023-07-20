// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Turret/PMBullet.h"

//Unreal
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

APMBullet::APMBullet(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<USphereComponent>(TriggerComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	////Create root and set it
	//m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//SetRootComponent(m_root);

	////Create Trigger comp
	//m_trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	//m_trigger->SetupAttachment(m_root);

	//Create Mesh comp
	m_bulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	m_bulletMesh->SetupAttachment(m_root);

	//Create Movement Comp
	m_projectileMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));

	//Set default value for speed, very slow
	m_projectileMoveComp->InitialSpeed = 100.f;
	m_projectileMoveComp->MaxSpeed = 500.f;

	//Set default life span
	SetLifeSpan(5.f);
}

void APMBullet::BeginPlay()
{
	Super::BeginPlay();
}

void APMBullet::OnTriggerComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// the owner should be the turret
	if (OtherActor == GetOwner()) return;

	RemoveBindTriggerComponentEvent();
	OnTrigger(OtherActor);
	OnHit(OtherActor, OtherComp, SweepResult);
	Destroy();
}

void APMBullet::OnHit(AActor* HitActor, UPrimitiveComponent* HitComponent, const FHitResult& SweepResult)
{
	ReceiveOnHit(HitActor, HitComponent, SweepResult);
}

void APMBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APMBullet::BeginDestroy()
{
	RemoveBindTriggerComponentEvent();

	Super::BeginDestroy();
}

