// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PM_PlayerControllerDemo.generated.h"

class UInputMappingContext;
class APM_CharacterDemo;

/**
 * Custom Player Controller for the Demo for the plugin 'Platformer Maker'
 */
UCLASS()
class PLATFORMERMAKER_API APM_PlayerControllerDemo : public APlayerController
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
private:
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<APM_CharacterDemo> m_demoCharacter;

	/*---------------------------------- FUNCTION ----------------------------------*/
public:
	APM_PlayerControllerDemo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BLueprintCallable)
	void AddInputMappingContext(const UInputMappingContext* InputMappingContext, const int32 Priority);

	UFUNCTION(BLueprintCallable)
	void RemoveInputMappingContext(const UInputMappingContext* InputMappingContext);

	UFUNCTION()
	FORCEINLINE void SetDemoCharacter(APM_CharacterDemo* InCharacter) { m_demoCharacter = InCharacter; }

	UFUNCTION()
	FORCEINLINE APM_CharacterDemo* GetDemoCharacter() const { return m_demoCharacter; }


	/*---------------------------------- OVERRIDE ----------------------------------*/
};