// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Tools/PMActorPlacer.h"
#include "PlatformerMaker.h"

//Unreal
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

APMActorPlacer::APMActorPlacer(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	m_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(m_root);

	m_splineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	m_splineComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APMActorPlacer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APMActorPlacer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APMActorPlacer::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SpawnActor();
}

#if WITH_EDITOR
void APMActorPlacer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SpawnActor();
}

void APMActorPlacer::PostEditUndo()
{
	Super::PostEditUndo();

	SpawnActor();
}

void APMActorPlacer::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	if (bFinished)
	{
		SpawnActor();
	}

}
#endif //WITH_EDITOR

void APMActorPlacer::SpawnActor()
{
	//Clear and remove old actor spawn
	if (m_actorPlaced.Num() > 0)
	{
		for (int i = m_actorPlaced.Num() - 1; i >= 0; i--)
		{
			m_actorPlaced[i]->Destroy();
		}

		m_actorPlaced.Empty();
	}

	//Check if all property is valid 
	if (!CanSpawnActors())
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, Can't spawn actors"), *GetName());

		return;
	}

	//Number of instance if check before
	//Should be higher than 0
	float lDistBetweenSpawn = m_numberOfInstances > 1 ? m_splineComponent->GetSplineLength() / (m_numberOfInstances - 1) : 0;
	float lCurrentDist = 0;

	FTransform lCurrentTransform;
	FVector lCurrentLoc;

	AActor* lCurrentActor = nullptr;

	for (int32 i = 0; i < m_numberOfInstances; i++)
	{
		lCurrentDist = lDistBetweenSpawn * i;

		lCurrentTransform = m_splineComponent->GetTransformAtDistanceAlongSpline(lCurrentDist, ESplineCoordinateSpace::World);

		lCurrentLoc = lCurrentTransform.GetLocation();

		lCurrentActor = GetWorld()->SpawnActor<AActor>(m_classToPlace, lCurrentLoc, FRotator());

		m_actorPlaced.Add(lCurrentActor);
	}
}

bool APMActorPlacer::CanSpawnActors()
{
	if (!IsValid(m_classToPlace))
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, Class to place not set"), *GetName());
		return false;
	}

	if (m_numberOfInstances <= 0)
	{
		UE_LOG(LogPlatformerPlugin, Log , TEXT("%s, No need to spawn due to 'Number of instance' set to 0"), *GetName());

		return false;
	}

	if (!IsValid(m_splineComponent))
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, Spline component is not valid"), *GetName());

		return false;
	}

	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogPlatformerPlugin, Error, TEXT("%s, there is no world set"), *GetName());

		return false;
	}

	return true;
}
