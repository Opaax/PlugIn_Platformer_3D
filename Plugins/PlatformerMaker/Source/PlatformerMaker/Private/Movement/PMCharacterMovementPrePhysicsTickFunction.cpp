#include "Movement/PMCharacterMovementPrePhysicsTickFunction.h"
#include "Movement/PMCharacterMovement.h"

void FPMCharacterMovementPrePhysicsTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	FActorComponentTickFunction::ExecuteTickHelper(Target, /*bTickInEditor=*/ false, DeltaTime, TickType, [this](float DilatedTime)
		{
			Target->PrePhysicsTickComponent(DilatedTime, *this);
		});
}

FString FPMCharacterMovementPrePhysicsTickFunction::DiagnosticMessage()
{
	return Target->GetFullName() + TEXT("[UPMCharacterMovement::PreClothTick]");
}

FName FPMCharacterMovementPrePhysicsTickFunction::DiagnosticContext(bool bDetailed)
{
	if (bDetailed) {
		return FName(*FString::Printf(TEXT("SkeletalMeshComponentClothTick/%s"), *GetFullNameSafe(Target)));
	}

	return FName(TEXT("SkeletalMeshComponentClothTick"));
}