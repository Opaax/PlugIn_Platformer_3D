// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMDamageableBar.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInstance;
class UImage;
class UOverlay;
class USizeBox;

/**
 * Base class for a damageable bar.
 */
UCLASS()
class PLATFORMERMAKER_API UPMDamageableBar : public UUserWidget
{
	GENERATED_BODY()
	
	/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damageable|Setting", meta = (DisplayName = "DamageableMaterial"))
	TObjectPtr<UMaterialInstance> m_damageableMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damageable|Setting", meta = (DisplayName = "TopProgressParameterName"))
	FName m_topProgressParameterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damageable|Setting", meta = (DisplayName = "BottomProgressParameterName"))
	FName m_bottomProgressParameterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damageable|Setting", meta = (DisplayName = "AnimationSpeed"))
	float m_animationSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, BlueprintSetter = SetTopProgress, Category = "Damageable|Runtime", meta = (DisplayName = "TopProgress"))
	float m_topProgress;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Damageable|Runtime", meta = (DisplayName = "BottomProgress"))
	float m_bottomProgress;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Damageable|Runtime", meta = (DisplayName = "PrevTopProgress"))
	float m_prevTopProgress;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Damageable|Runtime", meta = (DisplayName = "DamageableMaterialInstanceDynamic"))
	TObjectPtr<UMaterialInstanceDynamic> m_damageableInstMatDyn;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damageable", meta = (BindWidget))
	TObjectPtr<UImage> HealthImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damageable", meta = (BindWidget))
	TObjectPtr<UOverlay> OverlayContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damageable", meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBoxBar;

	/**************************** FUNCTIONS ******************************/
protected:
	UFUNCTION(BlueprintCallable)
	void SetImageMaterial();

	UFUNCTION(BlueprintCallable)
	void AnimLifeBar(const float DeltaTime);

public:
	//(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetBarMaterialParameters(const float TopProgress, const float BottomProgress);

	UFUNCTION(BlueprintCallable)
	void SetInitialProgressValues(const float TopProgress, const float BottomProgress);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTopProgress(const float NextProgress) { m_topProgress = NextProgress; }

	/**************************** OVERRIDE ******************************/
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

};
