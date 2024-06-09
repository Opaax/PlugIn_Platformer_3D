// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#include "Widget/PMUW_LoadingBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PMUW_LoadingBase)

UPMUW_LoadingBase::UPMUW_LoadingBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{

}

void UPMUW_LoadingBase::BP_BroadcastOnLoadingBeginAnimDone() const
{
	BroadcastOnLoadingBeginAnimDone_Internal();
}

void UPMUW_LoadingBase::BroadcastOnLoadingBeginAnimDone_Internal() const
{
	OnLoadingBeginAnimDone.Broadcast();
}

void UPMUW_LoadingBase::BP_BroadcastOnLoadingEndingAnimDone() const
{
	BroadcastOnLoadingEndingAnimDone_Internal();
}

void UPMUW_LoadingBase::StartLoadAnim()
{
	ReceiveStartLoadAnim();
}

void UPMUW_LoadingBase::StopLoadAnim()
{
	ReceiveStopLoadAnim();
}

void UPMUW_LoadingBase::BroadcastOnLoadingEndingAnimDone_Internal() const
{
	OnLoadingEndingAnimDone.Broadcast();
}