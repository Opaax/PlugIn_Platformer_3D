// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Turret/PMTurret.h"

//Unreal
#include "Perception/AIPerceptionComponent.h"

APMTurret::APMTurret(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

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
}

void APMTurret::OnSight(AActor* Actor)
{
	ReceiveOnSight(Actor);
}

// Called when the game starts or when spawned
void APMTurret::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APMTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

