#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{   // ź�� ȸ�� ���� ���� �������� ���Ÿ�
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Recovery UMETA(DisplayName = "Recovery"),
	EIT_Buff UMETA(DisplayName = "Buff"),
	EIT_MeleeWeapon UMETA(DisplayName = "Melee Weapon"),
	EIT_MagicMeleeWeapon UMETA(DisplayName = "MagicMelee Weapon"),
	EIT_RangedWeapon UMETA(DisplayName = "Ranged Weapon"),

	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};