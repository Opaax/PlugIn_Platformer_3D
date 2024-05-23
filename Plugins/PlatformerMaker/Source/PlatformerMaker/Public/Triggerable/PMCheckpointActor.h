// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable/PMTriggerableActor.h"
#include "PMCheckpointActor.generated.h"

class APlayerStart;
class UChildActorComponent;

/**
 * Simple Checkpoint actor for PM plugin
 */
UCLASS()
class PLATFORMERMAKER_API APMCheckpointActor : public APMTriggerableActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Checkpoint|PlayerStart", BlueprintGetter = "GetPlayerStart", BlueprintSetter = "SetPlayerStart")
	TObjectPtr<APlayerStart> m_playerStart;

	UPROPERTY(VisibleAnywhere, Category = "Checkpoint|PlayerStart")
	TObjectPtr<UChildActorComponent> m_childActorPlayerStart;

protected:
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SetPlayerStart(APlayerStart* NewPlayerStart) { m_playerStart = NewPlayerStart; }

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void CheckChildSetPlayerStart();

public:
	APMCheckpointActor(const FObjectInitializer& ObjectInit = FObjectInitializer::Get());

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Checkpoint")
	void ValidateCheckpoint();
	virtual void ValidateCheckpoint_Implementation();

	UFUNCTION(BlueprintPure, Category = "Checkpoint")
	FORCEINLINE APlayerStart* GetPlayerStart() const { return m_playerStart; }
	
protected:
	virtual void BeginPlay() override;
	virtual void OnTrigger(AActor* OtherTrigger) override;
};

/** GameModeBase events, particularly for use by plugins */
class PLATFORMERMAKER_API FCheckpointStaticEvent
{
public:

	/**
	 * Checkpoint Start Trigger
	 * - Called at the end of APMCheckpointActor::OnTrigger
	 *
	 * @param Checkpoint actor that has been Trigger
	 */
	DECLARE_EVENT_OneParam(APMCheckpointActor, FCheckpointTrigger, APMCheckpointActor* /*Checkpoint*/);

public:

	static FCheckpointTrigger& OnCheckpointTrigger() { return CheckpointTrigger; }

	static FCheckpointTrigger CheckpointTrigger;
};
