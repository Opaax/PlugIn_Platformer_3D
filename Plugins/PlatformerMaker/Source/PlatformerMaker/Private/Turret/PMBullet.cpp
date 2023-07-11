// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Turret/PMBullet.h"

//Unreal
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

APMBullet::APMBullet(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(m_root);

	m_trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	m_trigger->SetupAttachment(m_root);

	m_bulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	m_bulletMesh->SetupAttachment(m_root);

	m_projectileMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));

	m_projectileMoveComp->InitialSpeed = 100.f;
	m_projectileMoveComp->MaxSpeed = 500.f;

	SetLifeSpan(5.f);
}

// Called when the game starts or when spawned
void APMBullet::BeginPlay()
{
	Super::BeginPlay();

	BulletBeginPlay();
}

void APMBullet::BulletBeginPlay()
{
	m_trigger->OnComponentBeginOverlap.AddDynamic(this, &APMBullet::OnBoxComponentOverlapped);
}

void APMBullet::OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(m_trigger->OnComponentBeginOverlap.IsBound())
		m_trigger->OnComponentBeginOverlap.RemoveDynamic(this, &APMBullet::OnBoxComponentOverlapped);

	//Hit

	Destroy();
}

// Called every frame
void APMBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

