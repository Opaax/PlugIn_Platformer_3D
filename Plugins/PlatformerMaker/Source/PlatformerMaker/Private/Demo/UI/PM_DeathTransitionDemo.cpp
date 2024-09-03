// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Demo/UI/PM_DeathTransitionDemo.h"

UPM_DeathTransitionDemo::UPM_DeathTransitionDemo(const FObjectInitializer& ObjecyInit):Super(ObjecyInit)
{

}

void UPM_DeathTransitionDemo::StartTransition(const float SpeedAnim, const FDeathTransitionCallback& OnFinishCallback)
{
	ReceiveStartTransition(SpeedAnim, OnFinishCallback);
}