// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PMAcceleration.h"
#include "PMBraking.h"
#include "GameplayTagContainer.h"
#include "PMCharacterMovement.generated.h"

class UPMMovementMode;

/**
 * Custom Character movement component for Platformer Maker
 * 
 * Not attended to remplace Unreal Component, more likely readable component.
 * Use command pattern externalize movement phys algo in oher files.
 * 
 * Do not contains
 *	- Root motion
 *	- Net code
 */
UCLASS()
class PLATFORMERMAKER_API UPMCharacterMovement : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	/*---------------------------------- MEMBERS ----------------------------------*/
protected:
	UPROPERTY(EditAnywhere, Category = "Movement|Acceleration", BlueprintReadOnly, meta = (DisplayName = "Acceleration"))
	FPMAcceleration m_acceleration;

	UPROPERTY(EditAnywhere, Category = "Movement|Modes", BlueprintReadOnly, meta = (DisplayName = "AllMoveMode"))
	TMap<FGameplayTag, TSoftClassPtr<UPMMovementMode>> m_allMoveMode;

	UPROPERTY(EditAnywhere, Category = "Movement", BlueprintReadOnly, meta = (DisplayName = "CurrentMovementTag"))
	FGameplayTag m_currentMovementTag;

	/*---------------------------------- FUNCTION ----------------------------------*/
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "PerformMovement"))
	void PerformMovement(float DeltaTime);
	virtual void PerformMovement_Implementation(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "StartNewPhysics"))
	void StartNewPhysics(float DeltaTime, int32 Interation);
	virtual void StartNewPhysics_Implementation(float DeltaTime, int32 Interation);

	UFUNCTION(BlueprintNativeEvent, Category = "InternalEvents", meta = (DisplayName = "OnMoveTagChanged"))
	void OnMoveTagChanged();
	virtual void OnMoveTagChanged_Implementation();

public:
	UPMCharacterMovement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "Movement|Acceleration")
	FORCEINLINE FPMAcceleration GetAcceleration() const { return m_acceleration; }

	UFUNCTION(BlueprintCallable, Category = "Movement|Acceleration")
	void GetAccelerationRef(FPMAcceleration& OutAcceleration) const { OutAcceleration = m_acceleration; }

	UFUNCTION(BlueprintPure, Category = "Movement|Acceleration")
	FORCEINLINE FVector GetAccelerationVector() const { return m_acceleration.GetAcceleration(); }

	UFUNCTION(BlueprintCallable, Category = "Movement|Acceleration")
	void GetAccelerationVectorRef(FVector& OutAccelerationVector) const { OutAccelerationVector = m_acceleration.GetAcceleration(); }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool SetMovementTag(const FGameplayTag& NewMoveTag);

	/*---------------------------------- OVERRIDE ----------------------------------*/
public:
	//UObject
	virtual void PostLoad() override;

	//UActorComp
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Movement Comp
	virtual float GetGravityZ() const override;
	virtual bool IsMovingOnGround() const override;
	virtual bool IsFalling() const override;
	virtual float GetMaxSpeed() const override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
};
