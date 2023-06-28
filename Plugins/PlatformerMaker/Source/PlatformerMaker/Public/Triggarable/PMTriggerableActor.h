// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMTriggerableActor.generated.h"

class UShapeComponent;
class USceneComponent;

/*
* If you want to inherit this class, by default the trigger is a 'Shape Component'
* You can make it what ever shape you want by using 'TriggerComponentName' in the Constructor
* Like: APMBumperBase::APMBumperBase(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<UBoxComponent>(TriggerComponentName))
* 
* All inherited classe need to set it's trigger shape type
*/
UCLASS()
class PLATFORMERMAKER_API ATriggerableActor : public AActor
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:

	/*
	* Classes that can trigger this actor
	* It could be other actor such as character, boxes actor,...
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triggerable|Setting", meta = (DisplayName = "ValidClassToBump"))
	TArray<TSubclassOf<AActor>> m_validClassToBump;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShapeComponent> m_triggerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> m_root;

	/**************************** FUNCTION ******************************/
public:	
	ATriggerableActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static FName TriggerComponentName;
	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
