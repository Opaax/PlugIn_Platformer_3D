// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PM_CharacterDemo.generated.h"

//Forward

//PM Plugin
class APM_PlayerControllerDemo;

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

	UPROPERTY(EditDefaultsOnly,Category = "PM|Input", BlueprintGetter = "GetDefaultInputMappingContext", meta = (DisplayName = "DefaultInputMappingContext"))
	TObjectPtr<UInputMappingContext> m_defaultInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "PM|Input", BlueprintGetter = "GetDefaultInputMappingContextPriority", meta = (DisplayName = "DefaultInputMappingContextPriority"))
	int32 m_defaultInputMappingContextPriority;

	UPROPERTY(EditDefaultsOnly, Category = "PM|Input", meta = (DisplayName = "bAutoAddMappingContext"))
	bool bAutoAddMappingContext;

	UPROPERTY(VisibleInstanceOnly, Category = " PM|Runtime",BlueprintGetter = "GetPlayerControllerDemo", meta = (DisplayName = "bAutoAddMappingContext"))
	TObjectPtr<APM_PlayerControllerDemo> m_pmController;

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

	/*---------------------------------- GETTER / SETTER ----------------------------------*/
public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE UCapsuleComponent* GetCapsuleComponent() const { return m_capsuleComponent; }

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE UInputMappingContext* GetDefaultInputMappingContext() const { return m_defaultInputMappingContext; }

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE int32 GetDefaultInputMappingContextPriority() const { return m_defaultInputMappingContextPriority; }

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE APM_PlayerControllerDemo* GetPlayerControllerDemo() const { return m_pmController; }

	/*---------------------------------- OVERRIDE ----------------------------------*/
#pragma region Pawn_Override
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
#pragma endregion Pawn_Override
};
