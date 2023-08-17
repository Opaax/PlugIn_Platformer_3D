// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PMLifeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLifeComponentSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifeUpdateSignature, float, Life);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLATFORMERMAKER_API UPMLifeComponent : public UActorComponent
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:
	/*
	* Could be useful for debugging
	*/
	UPROPERTY(EditAnywhere, Category = "Life|Setting", BlueprintGetter = "IsStartWithMaxLife", meta = (DisplayName = "StartWithMaxLife"))
	bool bStartWithMaxLife = true;

	UPROPERTY(EditAnywhere, Category = "Life|Setting", BlueprintGetter = "GetStartLife", meta = (DisplayName = "StartLife", EditCondition = "!bStartWithMaxLife"))
	float m_startLife;

	UPROPERTY(EditAnywhere, Category = "Life|Setting", BlueprintGetter = "GetMaxLife", meta = (ClampMin = "0", DisplayName = "MaxLife"))
	float m_maxLife;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Life|Runtime", BlueprintGetter = "GetCurrentLife", meta = (DisplayName = "CurrentLife"))
	float m_currentLife;

public:
	UPROPERTY(BlueprintAssignable, Category = "Life|Delegate")
	FLifeComponentSignature OnNoMoreLife;

	UPROPERTY(BlueprintAssignable, Category = "Life|Delegate")
	FLifeUpdateSignature OnLifeUpdated;

	/**************************** FUNCTIONS ******************************/

public:
	UPMLifeComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#pragma region Blueprint Overridable

protected:
	/*
	* Call OnLifeUpdated in the first place
	* Then call OnNoMoreLife if life = 0;
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Life|Overridable")
	void CheckLife();
	virtual void CheckLife_Implementation();

public:
	/*
	* Add Life to CurrentLife
	* Will be clamp with 0 and Max Life
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Life|Overridable")
	void AddLife(float Amount);
	virtual void AddLife_Implementation(float Amount);

	/*
	* Remove Life to CurrentLife
	* Will be clamp with 0 and Max Life
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Life|Overridable")
	void RemoveLife(float Amount);
	virtual void RemoveLife_Implementation(float Amount);

#pragma endregion Function that can be overrided in BP

#pragma region Getter
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Life|Getter")
	FORCEINLINE bool IsStartWithMaxLife() const { return bStartWithMaxLife; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Life|Getter")
	FORCEINLINE float GetStartLife() const { return bStartWithMaxLife ? m_maxLife : m_startLife; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Life|Getter")
	FORCEINLINE float GetMaxLife() const { return m_maxLife; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Life|Getter")
	FORCEINLINE float GetCurrentLife() const { return m_currentLife; }
#pragma endregion Getter

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
