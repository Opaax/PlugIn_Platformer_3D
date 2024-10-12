// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMTriggerableActor.h"
#include "PMTrigger.generated.h"


/**
 * Base class for trigger.
 *
 * Can be used to set up multiple Trigger.
 * 
 */
UCLASS()
class PLATFORMERMAKER_API APMTrigger : public APMTriggerableActor
{
	GENERATED_UCLASS_BODY()

protected:
	/*
	 * This trigger can be edited to make it handle 'child' actor to trigger it.
	 */
	UPROPERTY(EditAnywhere, Category= "Trigger Setting", meta = (DisplayName = "ChildClass", MustImplement= "PMTriggerable"))
	TSubclassOf<AActor> m_childClass;
	
	UPROPERTY(EditAnywhere, Category= "Trigger Setting", meta = (DisplayName = "NumChildToSpawn"))
	int32 m_numChildToSpawn = 0;

	UPROPERTY(EditAnywhere, Category= "Trigger Setting", meta = (DisplayName = "SpacingBetweenChild", UIMin = "1.0", ClampMin = "1.0"))
	float m_spacingBetweenChild = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Trigger Setting", meta = ( DisplayName = "DelayBetweenChild", UIMin = "0.0", ClampMin = "0.0"))
	float m_delayBetweenChild = .4f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "SpawnChildren"))
	TArray<AActor*> m_spawnChildren;

protected:
#if WITH_EDITOR
	virtual void UpdateChildren();
	virtual void SpawnChildrenMissingChildren(const int32 Num);
#endif //WITH EDITOR
	
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
	virtual void PostEditMove(bool bFinished) override;
#endif//WITH_EDITOR
};
