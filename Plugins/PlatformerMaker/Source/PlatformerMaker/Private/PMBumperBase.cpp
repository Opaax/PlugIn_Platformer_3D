// Fill out your copyright notice in the Description page of Project Settings.


#include "PMBumperBase.h"

// Sets default values
APMBumperBase::APMBumperBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APMBumperBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APMBumperBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

