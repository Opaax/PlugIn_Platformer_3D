#pragma once

#include "CoreMinimal.h"
#include "PMAcceleration.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct PLATFORMERMAKER_API FPMAcceleration
{
	GENERATED_USTRUCT_BODY()

	/*---------------------------------- MEMBERS ----------------------------------*/
private:
	FVector m_acceleration;

public:
	UPROPERTY(Category = "Acceleration", EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", UIMin = "0"))
	float MaxAcceleration;

	/*---------------------------------- FUNCTIONS ----------------------------------*/
private:
	void UpdateAcceleration();

public:
	FPMAcceleration():
		MaxAcceleration(500.f)
	{
	}

	FPMAcceleration(const float InMaxAcceleration) :
		MaxAcceleration(InMaxAcceleration)
	{
	}

	FORCEINLINE FVector GetAcceleration() const { return m_acceleration; }

	void GetAccelerationRef(FVector& OutAcceleration) const { OutAcceleration = m_acceleration; }

	void SetAcceleration(FVector NewAcceleration) { m_acceleration = NewAcceleration; UpdateAcceleration(); }

	void AccumulateAccelerationRef(FVector NewAcceleration) { m_acceleration += NewAcceleration; UpdateAcceleration(); }

	FORCEINLINE void SetMaxAcceleration(float NewMaxAcceleration) { MaxAcceleration = NewMaxAcceleration; }
};