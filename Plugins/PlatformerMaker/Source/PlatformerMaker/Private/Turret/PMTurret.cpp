// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Turret/PMTurret.h"

//Unreal
#include "Perception/AIPerceptionComponent.h"
#include "Components/SceneComponent.h"

APMTurret::APMTurret(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(m_root);

	m_perceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
}

void APMTurret::ListenPerceptionComponentEvent()
{
	if (IsValid(m_perceptionComponent))
	{
		m_perceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &APMTurret::OnPerceptionUpdate);
	}
}

void APMTurret::OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	//Check perception
	//Check valid actor
	//Check Valid Sight actor
	//Call On Sight

	UE_LOG(LogTemp, Error, TEXT("TOORWEMWEF"));
	OnSight(Actor);
}

void APMTurret::OnSight(AActor* Actor)
{
	ReceiveOnSight(Actor);
}

// Called when the game starts or when spawned
void APMTurret::BeginPlay()
{
	Super::BeginPlay();

	ListenPerceptionComponentEvent();
	
}

// Called every frame
void APMTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

