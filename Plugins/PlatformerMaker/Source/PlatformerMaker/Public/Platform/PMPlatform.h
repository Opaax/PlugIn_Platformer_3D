// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMPlatform.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class PLATFORMERMAKER_API APMPlatform : public AActor
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
protected:

	UPROPERTY(VisibleAnywhere, Category = Component, meta = (allowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Mesh, meta = (allowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> m_meshPlatform;

	/**************************** FUNCTION ******************************/
public:	
	APMPlatform(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
