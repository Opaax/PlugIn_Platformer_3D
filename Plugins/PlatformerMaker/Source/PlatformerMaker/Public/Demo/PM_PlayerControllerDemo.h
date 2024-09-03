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
	UPROPERTY(VisibleInstanceOnly, Category= "Runtime")
	TObjectPtr<APM_CharacterDemo> m_demoCharacter;

	UPROPERTY(VisibleInstanceOnly, Category= "Runtime")
	TObjectPtr<APM_PlayerCameraManagerDemo> m_demoCamManager;

protected:
	UPROPERTY(VisibleInstanceOnly, Category= "Runtime", BlueprintGetter = "IsReturningToMenu", BlueprintSetter = "SetIsReturningToMenu", meta = (DisplayName = "bIsReturningToMenu"))
	bool bIsReturningToMenu = false;

	/*---------------------------------- FUNCTION ----------------------------------*/
private:
	void DestroyHUD();

	UFUNCTION(Category= "PlayerControllerDemo")
	void ProcessRestartDemo();

public:
	APM_PlayerControllerDemo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BLueprintCallable,Category= "PlayerControllerDemo")
	void AddInputMappingContext(const UInputMappingContext* InputMappingContext, const int32 Priority);

	UFUNCTION(BLueprintCallable,Category= "PlayerControllerDemo")
	void RemoveInputMappingContext(const UInputMappingContext* InputMappingContext);

	UFUNCTION(Category= "PlayerControllerDemo")
	FORCEINLINE void SetDemoCharacter(APM_CharacterDemo* InCharacter);

	UFUNCTION(Category= "PlayerControllerDemo")
	FORCEINLINE APM_CharacterDemo* GetDemoCharacter() const { return m_demoCharacter; }

	UFUNCTION(BlueprintCallable, BlueprintPure,Category= "PlayerControllerDemo")
	FORCEINLINE bool IsReturningToMenu() const { return bIsReturningToMenu;}
	
	UFUNCTION(BlueprintCallable,Category= "PlayerControllerDemo")
	FORCEINLINE void SetIsReturningToMenu(bool ReturningToMenu) { bIsReturningToMenu = ReturningToMenu;}


	/*---------------------------------- OVERRIDE ----------------------------------*/

#pragma region PlayerController
public:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	virtual void SpawnPlayerCameraManager() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PawnPendingDestroy(APawn* inPawn) override;
	virtual void SpawnDefaultHUD() override;
#pragma endregion PlayerController

};
