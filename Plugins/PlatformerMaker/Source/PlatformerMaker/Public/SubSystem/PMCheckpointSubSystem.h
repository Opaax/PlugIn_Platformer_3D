// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PMCheckpointSubsystem.generated.h"

class APMCheckpointActor;
class APlayerStart;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCheckpointSubSystemDelegate, APMCheckpointActor*, TriggeredCheckpoint, APlayerStart*, PlayerStart);

/**
 * Checkpoint subsystem work as manager for the PM plugin
 */
UCLASS()
class PLATFORMERMAKER_API UPMCheckpointSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	FDelegateHandle m_checkpointEventHandle;

	UPROPERTY(BlueprintGetter = "GetCurrentCheckPoint", Category= "Checkpoint|Runtime")
	TObjectPtr<APMCheckpointActor> m_currentCheckPoint;

public:

	UPROPERTY(BlueprintAssignable, Category = "Checkpoint|Delegates")
	FCheckpointSubSystemDelegate OnCheckpointChanged;
	
protected:
	UFUNCTION(Category = "CheckpointSubsystem")
	virtual void OnCheckpointTrigger(APMCheckpointActor* CheckpointTriggered);

public:
	UPMCheckpointSubsystem();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CheckpointSubsystem")
	FORCEINLINE APMCheckpointActor* GetCurrentCheckPoint() const { return m_currentCheckPoint; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CheckpointSubsystem")
	APlayerStart* GetCurrentCheckPointPlayerStart();

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
};
