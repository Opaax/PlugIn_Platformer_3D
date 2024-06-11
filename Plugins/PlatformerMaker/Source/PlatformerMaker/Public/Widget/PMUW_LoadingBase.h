// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMUW_LoadingBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWidgetLoadingSignature);

/**
 * USER WIDGET For default Loading Widget
 * 
 * This widget embark default event that help to call function when it right
 */
UCLASS()
class PLATFORMERMAKER_API UPMUW_LoadingBase : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Loading")
	FWidgetLoadingSignature OnLoadingBeginAnimDone;

	UPROPERTY(BlueprintAssignable, Category = "Loading")
	FWidgetLoadingSignature OnLoadingEndingAnimDone;

protected:
	UFUNCTION(BlueprintCallable, Category = "Loading", meta = (DisplayName = "BroadcastOnLoadingBeginAnimDone"))
	void BP_BroadcastOnLoadingBeginAnimDone() const;
	void BroadcastOnLoadingBeginAnimDone_Internal() const;

	UFUNCTION(BlueprintCallable, Category = "Loading", meta = (DisplayName = "BroadcastOnLoadingEndingAnimDone"))
	void BP_BroadcastOnLoadingEndingAnimDone() const;
	void BroadcastOnLoadingEndingAnimDone_Internal() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading", meta = (DisplayName = "StartLoadAnim"))
	void ReceiveStartLoadAnim();

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading", meta = (DisplayName = "StopLoadAnim"))
	void ReceiveStopLoadAnim();

public:
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void StartLoadAnim();

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void StopLoadAnim();
};
