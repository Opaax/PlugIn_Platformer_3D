// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PM_CharacterMovementDemo.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FPMAccelerationDemo {
	GENERATED_USTRUCT_BODY()

private:
	FVector m_acceleration;

public://explicit public
	UPROPERTY(Category = "Acceleration", EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", UIMin = "0"))
	float MaxAcceleration;

protected:
	void UpdateAcceleration();

public:
	FPMAccelerationDemo():
		m_acceleration(FVector::ZeroVector),
		MaxAcceleration(0)
	{}

	FPMAccelerationDemo(const FVector InAcceleration, const float InMaxAcceleration) :
		m_acceleration(InAcceleration),
		MaxAcceleration(InMaxAcceleration)
	{}

	FORCEINLINE float GetMaxAcceleration() const { return MaxAcceleration; }

	FORCEINLINE FVector GetAcceleration() const { return m_acceleration; }

	void GetAccelerationRef(FVector& OutAcceleration) const { OutAcceleration = m_acceleration; }

	void SetAccelerationRef(FVector NewAcceleration) { m_acceleration = NewAcceleration; UpdateAcceleration(); }

	void AccumulateAccelerationRef(FVector NewAcceleration) { m_acceleration += NewAcceleration; UpdateAcceleration(); }

	FORCEINLINE void SetMaxAcceleration(float NewMaxAcceleration) { MaxAcceleration = NewMaxAcceleration; }
};

/**
 * Character movement for the Demo
 */
UCLASS()
class PLATFORMERMAKER_API UPM_CharacterMovementDemo : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
private:
	UPROPERTY(EditAnywhere, Category = "Acceleration", meta = (AllowPrivateAccess = "True", DisplayName = "Acceleration"))
	FPMAccelerationDemo m_acceleration;

protected:
	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadOnly)
	TEnumAsByte<enum EMovementMode> m_movementMode;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
private:
	UFUNCTION()
	bool IsValidToMove();

	UFUNCTION()
	void CharacterControlledInput(const FVector& InputVector, float DeltaSeconds);

	UFUNCTION()
	FVector ContraintInputZ(FVector& InputVector);

	UFUNCTION()
	void PerformMovement(float DeltaSeconds);

	UFUNCTION()
	void OnMovementModeUpdated(EMovementMode NewMoveMode);

public:
	UPM_CharacterMovementDemo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Change movement mode.
	 *
	 * @param NewMovementMode	The new movement mode
	 * @param NewCustomMode		The new custom sub-mode, only applicable if NewMovementMode is Custom.
	 */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|Movement")
	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0);

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
