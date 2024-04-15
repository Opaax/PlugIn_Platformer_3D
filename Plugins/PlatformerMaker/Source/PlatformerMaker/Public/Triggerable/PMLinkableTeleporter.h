// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Triggerable/PMTriggerableActor.h"
#include "PMLinkableTeleporter.generated.h"

/**
 * Base Teleporter
 * 
 * This Teleporter Need a target teleport to be link to.
 * 
 */
UCLASS()
class PLATFORMERMAKER_API APMLinkableTeleporter : public APMTriggerableActor
{
	GENERATED_UCLASS_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
protected:
	UPROPERTY(Category = "Trigger Teleporter", BlueprintGetter = "GetCurrentTriggeActor", BlueprintSetter = "SetCurrentTriggeActor", meta = (DisplayName = "CurrentTriggerActor"))
	TObjectPtr<AActor> m_currentTriggerActor;

	UPROPERTY(EditAnywhere, Category = "Teleporter", meta = (DisplayName = "UseCustomTeleportLocation"))
	bool bUseCustomTeleportLocation = false;

	UPROPERTY(EditAnywhere, Category = "Teleporter", meta = (DisplayName = "TargetLocationTeleportation", EditConditionHides, EditCondition = "bUseCustomTeleportLocation", MakeEditWidget))
	FVector m_targetLocationTeleportationCustom;

	UPROPERTY(EditAnywhere,Category = "Teleporter", meta = (DisplayName = "UseCustomTeleportRotation"))
	bool bUseCustomTeleportRotation = false;

	UPROPERTY(EditAnywhere, Category = "Teleporter", meta = (DisplayName = "TargetRotationTeleportation", EditConditionHides, EditCondition = "bUseCustomTeleportRotation", MakeEditWidget))
	FRotator m_targetRotationTeleportationCustom;

public:
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Link Teleporter")
	TObjectPtr<APMLinkableTeleporter> TargetTeleporter;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
protected:
	UFUNCTION(BlueprintCallable)
	bool TeleportToLinkedTeleporter();

	/*---------------------------------- Getter / Setter ----------------------------------*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE AActor* GetCurrentTriggeActor() const { return m_currentTriggerActor; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentTriggeActor(AActor* const NewCurrentTriggerActor) { m_currentTriggerActor = NewCurrentTriggerActor; }

	/*---------------------------------- OVERRIDE ----------------------------------*/
protected:
	/*---------------------------------- AActor ----------------------------------*/
	virtual void BeginPlay() override;

	/*---------------------------------- APMTriggerable ----------------------------------*/
	virtual void OnTrigger(AActor* OtherTrigger) override;
};
