// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Door/PMDoor.h"

// Sets default values
APMDoor::APMDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APMDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APMDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

