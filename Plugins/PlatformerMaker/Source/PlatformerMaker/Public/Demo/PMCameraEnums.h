#pragma once

UENUM(BlueprintType)
enum class ECameraViewSide : uint8
{
	ECV_Invalid		UMETA(DisplayName = "Invalid"),
	ECV_Right		UMETA(DisplayName = "Right"),
	ECV_Left		UMETA(DisplayName = "Left"),
	ECV_Max			UMETA(Hidden)
};