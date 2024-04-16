// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PM_PlayerControllerDemo.generated.h"

class UInputMappingContext;
class APM_CharacterDemo;
class APM_PlayerCameraManagerDemo;

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

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<APM_PlayerCameraManagerDemo> m_demoCamManager;

	/*---------------------------------- FUNCTION ----------------------------------*/
private:
	void DestroyHUD();

	UFUNCTION()
	void ProcessRestartDemo();

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

#pragma region PlayerController
public:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	virtual void SpawnPlayerCameraManager();
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PawnPendingDestroy(APawn* inPawn) override;
	virtual void SpawnDefaultHUD() override;
#pragma endregion PlayerController

};
