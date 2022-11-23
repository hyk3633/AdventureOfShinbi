#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_None UMETA(DisplayName = "None"),
	EWT_Gun UMETA(DisplayName = "Gun"),
	EWT_Bow UMETA(DisplayName = "Bow"),
	EWT_MeleeOneHand UMETA(DisplayName = "Melee One Hand"),
	EWT_MeleeTwoHand UMETA(DisplayName = "Melee Two Hand"),
	EWT_Glave UMETA(DisplayName = "Glave"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ERangedWeaponType : uint8
{
	ERWT_Revenent UMETA(DisplayName = "Revenent"),
	ERWT_Wraith UMETA(DisplayName = "Wraith"),
	ERWT_AK47 UMETA(DisplayName = "AK47"),

	ERWT_MAX UMETA(DisplayName = "DefaultMAX")
};