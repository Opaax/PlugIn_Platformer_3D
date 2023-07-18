// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMActorPlacer.generated.h"

class USplineComponent;
class USceneComponent;

/*
* This tool is mainly made for placing coin
* Still you can place other actor as you need
*/
UCLASS()
class PLATFORMERMAKER_API APMActorPlacer : public AActor
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:

	UPROPERTY(EditAnywhere, Category = "Placer|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "ClassToPlace"))
	TSubclassOf<AActor> m_classToPlace;

	UPROPERTY(EditAnywhere, Category = "Placer|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "ReplaceOnMoveFinished"))
	bool bReplaceOnMoveFinished = false;
	
	UPROPERTY(EditAnywhere, Category = "Placer|Setting", meta = (AllowPrivateAccess = "true", DisplayName = "NumberOfInstances", ClampMin = "0"))
	int32 m_numberOfInstances = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placer|Debug", meta = (AllowPrivateAccess = "true", DisplayName = "ActorPlaced"))
	TArray<AActor*> m_actorPlaced;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> m_splineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	/**************************** FUNCTION ******************************/
protected:
	UFUNCTION(BlueprintCallable, Category = "PMPlacer")
	virtual void SpawnActor();

	UFUNCTION(Category = "Placer")
	virtual bool CanSpawnActors();

	UFUNCTION(BlueprintCallable, Category = "PMPlacer")
	virtual void ReplaceSpawnedActors();

	/*
	* Override this to make your custom begin play for this actor
	*/
	UFUNCTION(BlueprintCallable, Category = "PMPlacer")
	virtual void BeginPlayPlacer();

public:	
	APMActorPlacer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void PreEditUndo() override;
#endif//WITH_EDITOR
};
