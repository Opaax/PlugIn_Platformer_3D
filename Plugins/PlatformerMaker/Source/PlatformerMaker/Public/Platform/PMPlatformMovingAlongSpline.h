//2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Platform/PMPlatform.h"
#include "PMPlatformMovingAlongSpline.generated.h"

class APMSplineActor;
class UPMInterpSplineFollowMovement;

/**
 * This is a platform that follow a Spline perfectly.
 */
UCLASS(ClassGroup = "PM|Platform", Blueprintable, BlueprintType)
class PLATFORMERMAKER_API APMPlatformMovingAlongSpline : public APMPlatform
{
	GENERATED_UCLASS_BODY()
	
	/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(EditAnywhere, Category = "PM|Reference",BlueprintGetter = "GetSplineActor", BlueprintReadOnly, meta = (DisplayName = "SplineActor"))
	TObjectPtr<APMSplineActor> m_splineActor;

	UPROPERTY(VisibleAnyWhere, Category = "PM|Component", BlueprintGetter = "GetInterpSplineFollowComp")
	TObjectPtr<UPMInterpSplineFollowMovement> m_interpSplineFollowComp;

	/**************************** FUNCTION ******************************/
protected:
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE APMSplineActor* GetSplineActor() const { return m_splineActor; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE UPMInterpSplineFollowMovement* GetInterpSplineFollowComp() const { return m_interpSplineFollowComp; }

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void PreEditUndo() override;
#endif//WITH_EDITOR
};
