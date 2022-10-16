#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayeName = "Unoccupied"),
	ECS_Reloading UMETA(DisplayeName = "Reloading"),

	ECS_MAX UMETA(DisplayeName = "DefaultMAX")
};