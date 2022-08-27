#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{   // 탄약 회복 버프 근접 마법근접 원거리
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Recovery UMETA(DisplayName = "Recovery"),
	EIT_Buff UMETA(DisplayName = "Buff"),
	EIT_Bomb UMETA(DisplayName = "Bomb"),
	EIT_MeleeWeapon UMETA(DisplayName = "Melee Weapon"),
	EIT_MagicMeleeWeapon UMETA(DisplayName = "MagicMelee Weapon"),
	EIT_RangedWeapon UMETA(DisplayName = "Ranged Weapon"),

	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class ERecoveryType : uint8
{
	ERT_Health,
	ERT_Mana,
	ERT_Stamina,

	ERT_MAX UMETA(DisplayName = "DefaultMAX")
};

static FString SetItemTypeToWidget(EItemType Itemtype)
{
	switch (Itemtype)
	{
	case EItemType::EIT_MeleeWeapon:
		return TEXT("근접 무기");
	case EItemType::EIT_MagicMeleeWeapon:
		return TEXT("마법 근접 무기");
	case EItemType::EIT_RangedWeapon:
		return TEXT("원거리 무기");
	case EItemType::EIT_Bomb:
		return TEXT("폭탄");
	case EItemType::EIT_Ammo:
		return TEXT("탄약");
	case EItemType::EIT_Recovery:
		return TEXT("회복");
	case EItemType::EIT_Buff:
		return TEXT("강화");
	default:
		return TEXT("미지정");
	}
}