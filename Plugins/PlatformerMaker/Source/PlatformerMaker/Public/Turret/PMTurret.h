// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PMTurret.generated.h"

class UAIPerceptionComponent;

/*
*/
UCLASS()
class PLATFORMERMAKER_API APMTurret : public APawn
{
	GENERATED_BODY()
		/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> m_perceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "CurrentActor"))
	TObjectPtr<AActor> m_currentActor;

		/**************************** FUNCTION ******************************/
protected:
	UFUNCTION(BlueprintCallable)
	virtual void ListenPerceptionComponentEvent();

	UFUNCTION()
	void OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(BlueprintCallable)
	virtual void OnSight(AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSight"))
	void ReceiveOnSight(AActor* Actor);

public:
	APMTurret(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

		/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
