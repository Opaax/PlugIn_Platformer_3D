// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/PM_GameModeDemo.h"
#include "Demo/PM_HUDDemo.h"

APM_GameModeDemo::APM_GameModeDemo(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	HUDClass = APM_HUDDemo::StaticClass();
}