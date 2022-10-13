#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_Left UMETA(DisplayeName = "Turning Left"),
	ETIP_Right UMETA(DisplayeName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplayeName = "Not Turning"),

	ETIP_MAX UMETA(DisplayeName = "DefaultMAX")
};