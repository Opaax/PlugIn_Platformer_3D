#pragma once

UENUM(BlueprintType)
enum class EPMDoorState : uint8
{
	EDS_Open UMETA(DisplayName = "Open"),
	EDS_Close UMETA(DisplayName = "Close"),
	EDS_OnOpening UMETA(DisplayName = "OnOpening"),
	EDS_OnClosing UMETA(DisplayName = "OnClosing")
};

UENUM(BlueprintType)
enum class EPMDoorStartState : uint8
{
	EDSS_Open UMETA(DisplayName = "StartOpen"),
	EDSS_Close UMETA(DisplayName = "StartClose")
};