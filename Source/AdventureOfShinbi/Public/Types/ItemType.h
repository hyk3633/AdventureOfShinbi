#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{   // 탄약 회복 버프 근접 마법근접 원거리
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Recovery UMETA(DisplayName = "Recovery"),
	EIT_Buff UMETA(DisplayName = "Buff"),
	EIT_MeleeWeapon UMETA(DisplayName = "Melee Weapon"),
	EIT_MagicMeleeWeapon UMETA(DisplayName = "MagicMelee Weapon"),
	EIT_RangedWeapon UMETA(DisplayName = "Ranged Weapon"),

	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};