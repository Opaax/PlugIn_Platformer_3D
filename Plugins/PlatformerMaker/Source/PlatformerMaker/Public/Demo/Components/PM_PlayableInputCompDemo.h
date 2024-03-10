// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "PM_PlayableInputCompDemo.generated.h"

//Demo Forward
class APM_PlayerControllerDemo;
class APM_CharacterDemo;
class UPM_InputPlayerConfig;

//Unreal Forward
class UInputMappingContext;


USTRUCT(BlueprintType)
struct FInputMappingContextAndPriority
{
	GENERATED_USTRUCT_BODY()

	/*---------------------------------- MEMBERS ----------------------------------*/
public://explicit public even struct is public by default
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 InputPriority;

	/*
	* If true, then this mapping context will be registered with the settings when this game feature action is registered.
	* No used in the demo but it could be relevent to you if you using User setting
	*/
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bRegisterWithSettings = true;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
public://explicit public even struct is public by default
	FInputMappingContextAndPriority() :
		MappingContext(nullptr),
		InputPriority(-1)
	{
		bRegisterWithSettings = false;
	}

	explicit FInputMappingContextAndPriority(UInputMappingContext* const InMappingContext, int32 const InPriority) :
		MappingContext(InMappingContext),
		InputPriority(InPriority)
	{
		bRegisterWithSettings = false;
	}

	explicit FInputMappingContextAndPriority(UInputMappingContext* const InMappingContext, int32 const InPriority, bool RegisterWithSettings) :
		MappingContext(InMappingContext),
		InputPriority(InPriority),
		bRegisterWithSettings(RegisterWithSettings)
	{
	}

	/*---------------------------------- OVERRIDE ----------------------------------*/
};

/**
 * 
 */
UCLASS(Category = "Demo")
class PLATFORMERMAKER_API UPM_PlayableInputCompDemo : public UPawnComponent
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
private:
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (DisplayName = "DefaultMappingContext"))
	FInputMappingContextAndPriority m_defaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (DisplayName = "InputConfig"))
	TObjectPtr<UPM_InputPlayerConfig> m_inputConfig;

	UPROPERTY()
	TArray<uint32> m_bindHandles;

	UPROPERTY()
	TObjectPtr<APM_CharacterDemo> m_demoCharacter;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
	UFUNCTION()
	void Input_Movement_Internal(const FInputActionValue& InputActionValue);

public:
	UPM_PlayableInputCompDemo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void InitializePlayerInput(APM_PlayerControllerDemo* PlayerControllerDemo);
	virtual void InitializePlayerInput_Implementation(APM_PlayerControllerDemo* PlayerControllerDemo);

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	virtual void BeginPlay() override;
};
