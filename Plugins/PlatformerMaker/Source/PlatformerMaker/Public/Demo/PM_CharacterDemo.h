// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagAssetInterface.h"
#include "AbilitySystemInterface.h"
#include "PM_CharacterDemo.generated.h"

//Forward

//PM Plugin
class APM_PlayerControllerDemo;
class UPM_PlayableInputCompDemo;
class UPM_CharacterMovementDemo;

//Unreal
class UCapsuleComponent;
class UInputMappingContext;
class UArrowComponent;
class UAbilitySystemComponent;


/* 
* Custom Pawn for the Demo for the plugin 'Platformer Maker'
*/
UCLASS(Config=DemoGame)
class PLATFORMERMAKER_API APM_CharacterDemo : public APawn, public IGameplayTagAssetInterface, public IAbilitySystemInterface
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

	UPROPERTY(VisibleAnywhere, Category = " PM|Components", BlueprintGetter = "GetAbilitySystemComp")
	TObjectPtr<UAbilitySystemComponent> m_abilitySystemComp;

	UPROPERTY(VisibleAnywhere, Category = " PM|Components", BlueprintGetter = "CharacterMovement")
	TObjectPtr<UPM_CharacterMovementDemo> m_characterMovement;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UArrowComponent> m_arrowComponent;
#endif

public:
	static FName CapsuleComponentName;
	static FName CharacterMovementComponentName;

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

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE UAbilitySystemComponent* GetAbilitySystemComp() const { return m_abilitySystemComp; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE UPM_CharacterMovementDemo* CharacterMovement() const { return m_characterMovement; }
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
	virtual UPawnMovementComponent* GetMovementComponent() const override;

#pragma endregion Pawn_Override

#pragma region Tag_Interface_Override
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
#pragma endregion Tag_Interface_Override

#pragma region Ability_Interface_Override
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
#pragma endregion Ability_Interface_Override
};
