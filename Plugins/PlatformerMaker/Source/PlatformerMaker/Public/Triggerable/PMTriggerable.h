// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PMTriggerable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPMTriggerable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PLATFORMERMAKER_API IPMTriggerable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Triggerable)
	void OnTrigger(const float Delay);
	void OnTrigger_Implementation(const float Delay) {}

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Triggerable)
	void OnStopTrigger();
	void OnStopTrigger_Implementation() {}
};
