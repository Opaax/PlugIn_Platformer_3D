// Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PMScoreSubSystem.generated.h"

class IConsoleCommande;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreSubsystemSignature, int32, Score);

/**
 * Subsystem to handle score.
 * This system is very simple, you can add feature such as getting score from save.
 * You can allow the system spawn in Project setting - Plugin - Platformer Maker
 */
UCLASS()
class PLATFORMERMAKER_API UPMScoreSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

	/**************************** MEMBERS ******************************/
private:
	TArray<IConsoleObject*> m_cmdSubScore;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Score", BlueprintGetter = "GetCurrentScore", meta = (DisplayName = "CurrentScore"))
	int32 m_currentScore = 0;

public:
	UPROPERTY(BlueprintAssignable, Category = "Score")
	FScoreSubsystemSignature OnScoreUpdated;

	/**************************** FUNCTION ******************************/
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable ,Category = "Score")
	void AddScore(int32 AddScore);
	virtual void AddScore_Implementation(int32 AddScore);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Score")
	void RemoveScore(int32 RemoveScore);
	virtual void RemoveScore_Implementation(int32 RemoveScore);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
	FORCEINLINE int32 GetCurrentScore() const { return m_currentScore; }

	/**************************** OVERRIDE ******************************/
public:
	virtual bool ShouldCreateSubsystem (UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**************************** Debug ******************************/
#if WITH_EDITOR
public:
	void DebugAddScore(const TArray<FString>& Args);
	void DebugRemoveScore(const TArray<FString>& Args);
#endif
};
