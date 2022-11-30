#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_None UMETA(DisplayName = "None"),
	EWT_ShinbiSword UMETA(DisplayName = "Shinbi Sword"),
	EWT_Glaive UMETA(DisplayName = "Glaive"),
	EWT_Revenent UMETA(DisplayName = "Revenent"),
	EWT_Wraith UMETA(DisplayName = "Wraith"),
	EWT_AK47 UMETA(DisplayName = "AK47"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};