// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMLifeBar.generated.h"

class UPMDamageableBar;
class UOverlay;

/**
 * Base class for a life bar
 */
UCLASS()
class PLATFORMERMAKER_API UPMLifeBar : public UUserWidget
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Life)
	float m_maxLife = 100;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Life)
	float m_currentLife = 100;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Life, meta = (BindWidget))
	TObjectPtr<UPMDamageableBar> LifeBar;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Life, meta = (BindWidget))
	TObjectPtr<UOverlay> OverlayContainer;

	/**************************** FUNCTION ******************************/
public:
	/*
	* Init default values
	*/
	UFUNCTION(BlueprintCallable, Category = "LifeBar")
	void InitializeValues(const float MaxLife, const float CurrentLife);

	/*
	* Update the life bar
	* 
	* @NextLife: will set current life internal
	* @LifeRatio: value between 0 - 1
	*/
	UFUNCTION(BlueprintCallable, Category = "LifeBar")
	void UpdateLife(const float NextLife, const float LifeRatio);	
};
