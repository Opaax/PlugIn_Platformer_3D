// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "../Public/Platform/PMPlatform.h"

// Sets default values
APMPlatform::APMPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APMPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APMPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

