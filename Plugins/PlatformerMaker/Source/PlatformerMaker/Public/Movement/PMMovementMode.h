// 2024 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "PMMovementMode.generated.h"

class UPMCharacterMovement;

/**
 * Base movement mode for the PM character Movement
 */
UCLASS(abstract, BlueprintType, Blueprintable, Category = "Movement")
class PLATFORMERMAKER_API UPMMovementMode : public UObject
{
	GENERATED_BODY()

	/*---------------------------------- MEMBERS ----------------------------------*/
protected:
	UPROPERTY()
	TObjectPtr<UPMCharacterMovement> m_currentMoveComp;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "MovementTag", DisplayName = "MovementTag"))
	FGameplayTag MovementTag;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
protected:
	UFUNCTION()
	void SetMoveComp_Internal(UPMCharacterMovement* InMoveComp);

public:
	UPMMovementMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "PhysMovement"))
	bool PhysMovement(float DeltaTime, int32 Iterations, UPMCharacterMovement* MoveComp);
	virtual bool PhysMovement_Implementation(float DeltaTime, int32 Iterations, UPMCharacterMovement* MoveComp) { return false; };

	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "StartMovement"))
	bool StartMovement(float DeltaTime, const UPMCharacterMovement* MoveComp);
	virtual bool StartMovement_Implementation(float DeltaTime, const UPMCharacterMovement* MoveComp) { return true; };

	UFUNCTION(BlueprintNativeEvent, Category = "MovementMode", meta = (DisplayName = "GetMaxSpeed"))
	float GetMaxSpeed();
	virtual float GetMaxSpeed_Implementation() { return 0; };

	/*---------------------------------- OVERRIDE ----------------------------------*/

};
