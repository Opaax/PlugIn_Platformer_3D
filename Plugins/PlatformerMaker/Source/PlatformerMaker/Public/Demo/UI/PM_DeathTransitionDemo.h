// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Demo/PM_DemoCoreDelegates.h"
#include "PM_DeathTransitionDemo.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERMAKER_API UPM_DeathTransitionDemo : public UUserWidget
{
	GENERATED_UCLASS_BODY()

	/*---------------------------------- MEMBERS ----------------------------------*/
	/*---------------------------------- FUNCTIONS ----------------------------------*/
public:
	virtual void StartTransition(const float SpeedAnim, const FDeathTransitionCallback& OnFinishCallback);

	UFUNCTION(BlueprintImplementableEvent, Category = "Event", meta = (DisplayName = "StartTransition"))
	void ReceiveStartTransition(const float SpeedAnim, const FDeathTransitionCallback& OnFinishCallback);
	/*---------------------------------- OVERRIDE ----------------------------------*/
};
