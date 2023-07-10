// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Platform/PMMovingPlatform.h"
#include "PMTransporterPlatform.generated.h"

class UBoxComponent;
class ACharacter;

/**
 * This is a special moving platform
 * This platform will move only if the player climb up
 * This can have more function such as "Reset" when player get out of the trigger (going back to the start pos)
 */
UCLASS()
class PLATFORMERMAKER_API APMTransporterPlatform : public APMMovingPlatform
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
protected:
	UPROPERTY(VisibleAnywhere, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> m_triggerComponent;

	/*
	* Classes that can trigger this actor
	* It could be other actor such as character, boxes actor,...
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform|Setting", meta = (DisplayName = "ValidClassToTrigger"))
	TArray<TSubclassOf<ACharacter>> m_validCharacterClassToTrigger;

	UPROPERTY(VisibleInstanceOnly, Category = "Platform|Runtime", meta = (AllowPrivateAccess = "true", DisplayName = "IsOnStart"))
	bool bIsOnStart = true;

	/**************************** FUNCTION ******************************/
protected:
	UFUNCTION(BlueprintCallable)
	virtual void OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnPlatformTrigger();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPlatformTrigger"))
	void ReceiveOnPlatformTrigger();

	UFUNCTION(BlueprintCallable)
	virtual bool CanTrigger(AActor* OtherActor);

	UFUNCTION()
	void OnInterpComponentStop(const FHitResult& ImpactResult, float Time);

public:
	APMTransporterPlatform(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EnableMovingPlatform(bool NewEnable);

	UFUNCTION(BlueprintCallable)
	virtual void BindBoxComponentEvent();

	/**************************** OVERRIDE ******************************/
protected:
	virtual void BeginPlay() override;
};
