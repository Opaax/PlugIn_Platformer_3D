// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "PMAIPerceptionComponent.generated.h"

/**
 * I need to make a custom Perception to get data from this
 * By default all the the data  is protected/private
 * I write 2 getter for this
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLATFORMERMAKER_API UPMAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()
	
protected:
	virtual void OnRegister() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Return the Sight angle from sense sight config
	* 0 if not found
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Getter")
	virtual float GetSightAngle() const;

	/*
	* Return the Sight radius from sense sight config
	* 0 if not found
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Getter")
	virtual float GetSightRadius() const;

};
