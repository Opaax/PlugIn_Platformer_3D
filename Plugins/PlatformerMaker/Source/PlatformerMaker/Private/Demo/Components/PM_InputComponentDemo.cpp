// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Demo/Components/PM_InputComponentDemo.h"

UPM_InputComponentDemo::UPM_InputComponentDemo(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

void UPM_InputComponentDemo::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles) {
		RemoveBindingByHandle(Handle);
	}

	BindHandles.Reset();
}