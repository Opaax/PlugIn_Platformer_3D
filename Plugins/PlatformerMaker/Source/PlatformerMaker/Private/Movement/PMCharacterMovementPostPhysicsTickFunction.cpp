#include "Movement/PMCharacterMovementPostPhysicsTickFunction.h"
#include "Movement/PMCharacterMovement.h"

void FPMCharacterMovementPostPhysicsTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	FActorComponentTickFunction::ExecuteTickHelper(Target, /*bTickInEditor=*/ false, DeltaTime, TickType, [this](float DilatedTime)
		{
			Target->PostPhysicsTickComponent(DilatedTime, *this);
		});
}

FString FPMCharacterMovementPostPhysicsTickFunction::DiagnosticMessage()
{
	return Target->GetFullName() + TEXT("[UPMCharacterMovement::PreClothTick]");
}

FName FPMCharacterMovementPostPhysicsTickFunction::DiagnosticContext(bool bDetailed)
{
	if (bDetailed) {
		return FName(*FString::Printf(TEXT("SkeletalMeshComponentClothTick/%s"), *GetFullNameSafe(Target)));
	}

	return FName(TEXT("SkeletalMeshComponentClothTick"));
}