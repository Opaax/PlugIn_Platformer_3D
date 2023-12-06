//2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMSplineActor.generated.h"

class USplineComponent;

/*
* Is A simple Actor that contains a spline component
*/
UCLASS(ClassGroup = "PM|Tool", Blueprintable, BlueprintType, meta = (ChildCannotTick))
class PLATFORMERMAKER_API APMSplineActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
	/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(EditAnywhere, Category = "PM|Setting", BlueprintReadOnly, meta = (AllowPrivateAcces = "true", DisplayName = "DestroyAtStart"))
	bool bDestroyAtStart;

	UPROPERTY(VisibleAnywhere, Category = "PM|Component", BlueprintReadOnly, meta = (AllowPrivateAcces = "true"))
	TObjectPtr<USplineComponent> m_splineComp;

	/**************************** FUNCTIONS ******************************/
protected:
	UFUNCTION(BlueprintCallable, Category = "PM|SplineActor")
	void CheckDestroyAtBeginPlay();

	UFUNCTION(BlueprintCallable, Category = "PM|Getter")
	FORCEINLINE USplineComponent* GetSplineComponent() const { return m_splineComp; }

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

};
