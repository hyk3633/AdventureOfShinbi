
#include "System/AOSGameInstance.h"
#include "System/AOSGameModeBase.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
#include "Items/ItemRecovery.h"
#include "Items/ItemAmmo.h"
#include "Kismet/GameplayStatics.h"

void UAOSGameInstance::SavePlayerData()
{
	SavePlayerInfo();

	SaveWeaponInfo();
	
	SaveItemInfo();
}

FWeaponInfo UAOSGameInstance::GetWeaponInfo(int32 Index)
{
	return WeaponInfoArr[Index];
}

FItemInfo UAOSGameInstance::GetItemInfo(int32 Index)
{
	return ItemInfoArr[Index];
}

int32 UAOSGameInstance::GetWeaponInfoCount()
{
	return WeaponInfoArr.Num();
}

int32 UAOSGameInstance::GetItemInfoCount()
{
	return ItemInfoArr.Num();
}

float UAOSGameInstance::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UAOSGameInstance::GetCurrentMana() const
{
	return CurrentMana;
}

bool UAOSGameInstance::GetIsArmed() const
{
	return bIsArmed;
}

void UAOSGameInstance::SavePlayerInfo()
{
	AAOSCharacter* Player = Cast<AAOSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld()->GetFirstPlayerController(), 0));
	if (Player == nullptr)
		return;

	CurrentHealth = Player->GetCombatComp()->GetHealth();
	CurrentMana = Player->GetCombatComp()->GetMana();
}

void UAOSGameInstance::SaveWeaponInfo()
{
	WeaponInfoArr.Empty();

	AAOSGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AAOSGameModeBase>();
	if (GameMode == nullptr)
		return;

	if (GameMode->GetWeaponCount() == 0)
		return;

	for (int32 i = 0; i < GameMode->GetWeaponCount(); i++)
	{
		FWeaponInfo WeaponInfo;

		AWeapon* TempWeapon = GameMode->GetWeaponItem(i);

		WeaponInfo.WeaponType = TempWeapon->GetWeaponType();

		if (TempWeapon == GameMode->GetEquippedWeapon())
		{
			WeaponInfo.SlotType = ESlotType::EST_Equip;
			bIsArmed = true;
		}
		else if (TempWeapon == GameMode->GetQuickSlot1Weapon())
		{
			WeaponInfo.SlotType = ESlotType::EST_Quick1;
		}
		else if (TempWeapon == GameMode->GetQuickSlot2Weapon())
		{
			WeaponInfo.SlotType = ESlotType::EST_Quick2;
		}
		else
		{
			WeaponInfo.SlotType = ESlotType::EST_None;
		}

		ARangedWeapon* RW = Cast<ARangedWeapon>(TempWeapon);
		WeaponInfo.LoadedAmmo = RW == nullptr ? -1 : RW->GetLoadedAmmo();

		WeaponInfoArr.Add(WeaponInfo);
	}
}

void UAOSGameInstance::SaveItemInfo()
{
	ItemInfoArr.Empty();

	AAOSGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AAOSGameModeBase>();
	if (GameMode == nullptr)
		return;

	if (GameMode->GetItemCount() == 0)
		return;

	for (int32 i = 0; i < GameMode->GetItemCount(); i++)
	{
		FItemInfo ItemInfo;

		AItem* TempItem = GameMode->GetItem(i);

		ItemInfo.ItemType = TempItem->GetItemType();
		
		if (ItemInfo.ItemType == EItemType::EIT_Ammo)
		{
			AItemAmmo* Ammo = Cast<AItemAmmo>(TempItem);
			if (Ammo)
			{
				ItemInfo.ItemDetailType = static_cast<int8>(Ammo->GetAmmoType());
				ItemInfo.ItemCount = GameMode->GetAmmoQuantity(Ammo->GetAmmoType());
				ItemInfo.QuickSlotIndex = -1;
				ItemInfo.bIsEquipped = false;
			}
		}
		else if (ItemInfo.ItemType == EItemType::EIT_Recovery)
		{
			AItemRecovery* Recovery = Cast<AItemRecovery>(TempItem);
			if (Recovery)
			{
				ItemInfo.ItemDetailType = static_cast<int8>(Recovery->GetRecoveryType());
				ItemInfo.ItemCount = GameMode->GetRecoveryItemCount(Recovery->GetRecoveryType());
				ItemInfo.QuickSlotIndex = Recovery->GetQuickSlotIndex();
				ItemInfo.bIsEquipped = ItemInfo.QuickSlotIndex == GameMode->GetActivatedQuickSlotNumber() ? true : false;
			}
		}

		ItemInfoArr.Add(ItemInfo);
	}
}

void UAOSGameInstance::AcitavateShouldLoadData()
{
	bShouldLoadData = true;
}

void UAOSGameInstance::DeacitavateShouldLoadData()
{
	bShouldLoadData = false;
}

bool UAOSGameInstance::GetShouldLoadData() const
{
	return bShouldLoadData;
}