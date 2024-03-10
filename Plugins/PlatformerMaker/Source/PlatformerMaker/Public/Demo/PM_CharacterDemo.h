// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PM_CharacterDemo.generated.h"

//Forward

//PM Plugin
class APM_PlayerControllerDemo;
class UPM_PlayableInputCompDemo;

//Unreal
class UCapsuleComponent;
class UInputMappingContext;
class UArrowComponent;


/* 
* Custom Pawn for the Demo for the plugin 'Platformer Maker'
*/
UCLASS(Config=DemoGame)
class PLATFORMERMAKER_API APM_CharacterDemo : public APawn
{
	GENERATED_BODY()

	/*---------------------------------- MEMBERS ----------------------------------*/
private:
	UPROPERTY(VisibleAnywhere, BlueprintGetter = "GetCapsuleComponent", meta = (AllowPrivateAccess = "True"))
	TObjectPtr<UCapsuleComponent> m_capsuleComponent;

	UPROPERTY(VisibleInstanceOnly, Category = " PM|Runtime",BlueprintGetter = "GetPlayerControllerDemo", meta = (DisplayName = "bAutoAddMappingContext"))
	TObjectPtr<APM_PlayerControllerDemo> m_pmController;

	UPROPERTY(VisibleAnywhere, Category = " PM|Components", BlueprintGetter = "GetPlayableInputComp")
	TObjectPtr<UPM_PlayableInputCompDemo> m_playableInputComp;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UArrowComponent> m_arrowComponent;
#endif

public:
	static FName CapsuleComponentName;

	/*---------------------------------- FUNCTION ----------------------------------*/
private:
	UFUNCTION()
	void CheckAutoAddMappingContext();

public:
	APM_CharacterDemo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION()
	void Input_Movement(const FInputActionValue& InputActionValue);

	/*---------------------------------- GETTER / SETTER ----------------------------------*/
public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE UCapsuleComponent* GetCapsuleComponent() const { return m_capsuleComponent; }

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE APM_PlayerControllerDemo* GetPlayerControllerDemo() const { return m_pmController; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE UPM_PlayableInputCompDemo* GetPlayableInputComp() const { return m_playableInputComp; }

	/*---------------------------------- OVERRIDE ----------------------------------*/
#pragma region Pawn_Override
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PawnClientRestart() override;
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;
#pragma endregion Pawn_Override
};
