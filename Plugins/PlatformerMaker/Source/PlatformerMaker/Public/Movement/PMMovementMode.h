// Copyright Enguerran COBERT, Inc. All Rights Reserved.

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
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "MovementTag", DisplayName = "MovementTag"))
	FGameplayTag MovementTag;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
public:
	UPMMovementMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "PhysMovement"))
	bool PhysMovement(float DeltaTime, int32 Iterations, UPMCharacterMovement* MoveComp);
	virtual bool PhysMovement_Implementation(float DeltaTime, int32 Iterations, UPMCharacterMovement* MoveComp) { return false; };

	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "StartMovement"))
	bool StartMovement(float DeltaTime, const UPMCharacterMovement* MoveComp);
	virtual bool StartMovement_Implementation(float DeltaTime, const UPMCharacterMovement* MoveComp) { return true; };

	/*---------------------------------- OVERRIDE ----------------------------------*/

};
