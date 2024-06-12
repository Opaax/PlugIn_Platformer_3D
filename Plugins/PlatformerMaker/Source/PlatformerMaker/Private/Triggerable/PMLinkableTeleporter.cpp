// Copyright Enguerran COBERT, Inc. All Rights Reserved.


#include "Triggerable/PMLinkableTeleporter.h"
#include "Utils/DebugMacro.h"
#include "PlatformerMaker.h"

APMLinkableTeleporter::APMLinkableTeleporter(const FObjectInitializer& ObjectInit) :Super(ObjectInit)
{

}

bool APMLinkableTeleporter::TeleportToLinkedTeleporter()
{
	if (IsValid(TargetTeleporter) && IsValid(m_currentTriggerActor)) {
		FVector lTeleportLocation	= TargetTeleporter->GetActorLocation();
		const FRotator lTeleportRotation	= bUseCustomTeleportRotation ? m_targetRotationTeleportationCustom : TargetTeleporter->GetActorRotation();

		// We use Widget MakeEditWidget on m_targetLocationTeleportationCustom. 
		// This is a bit confusing but it show the widget location on local space, but when we use it its global space.
		// Like this we add off set location if needed on actor location which is global space
		lTeleportLocation += bUseCustomTeleportLocation ? m_targetLocationTeleportationCustom : FVector::ZeroVector;

		return m_currentTriggerActor->TeleportTo(lTeleportLocation, lTeleportRotation, false);
	}

	return false;
}

void APMLinkableTeleporter::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetTeleporter) {
		DEBUG_ERROR_CUSTOM_CATEGORY(LogPlatformerPlugin, TEXT("[%s], Do not Have Target Teleporter"), *GetNameSafe(this));
	}
}

void APMLinkableTeleporter::OnTrigger(AActor* OtherTrigger)
{
	Super::OnTrigger(OtherTrigger);

	SetCurrentTriggerActor(OtherTrigger);
}