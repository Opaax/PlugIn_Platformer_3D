// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Public/Triggarable/PMTriggerableActor.h"
#include "PMCoin.generated.h"

class UStaticMeshComponent;

UCLASS()
class PLATFORMERMAKER_API APMCoin : public ATriggerableActor
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
protected:

	UPROPERTY(Visibleanywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> m_coinMesh;

	/**************************** FUNCTION ******************************/
public:
	APMCoin(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** MEMBERS ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
