
#include "System/AOSGameInstance.h"
#include "System/AOSGameModeBase.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
#include "Items/ItemRecovery.h"
#include "Items/ItemAmmo.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/ObjectLibrary.h"

void UAOSGameInstance::SetAsyncLoad()
{
	if (!ObjectLibrary)
	{
		ObjectLibrary = UObjectLibrary::CreateLibrary(USkeletalMesh::StaticClass(), false, GIsEditor);
		ObjectLibrary->AddToRoot();
	}
	int32 AssetNumber = ObjectLibrary->LoadAssetDataFromPath(TEXT("/Game/_Assets/SkeletalMesh"));
	if (AssetNumber > 0)
	{
		ObjectLibrary->LoadAssetsFromAssetData();
	}

	ObjectLibrary->GetAssetDataList(AssetDatas);
}

FString UAOSGameInstance::GetAssetReference(FString AssetName)
{
	for (int32 i = 0; i < AssetDatas.Num(); ++i)
	{
		FAssetData& AssetData = AssetDatas[i];

		if (AssetData.AssetName.ToString().Contains(AssetName))
		{
			return AssetData.ToSoftObjectPath().GetAssetPathString();
		}
	}

	return FString(TEXT(""));
}

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

	if (GameMode->AcquiredWeapons.Num() == 0)
		return;

	for (int32 i = 0; i < GameMode->AcquiredWeapons.Num(); i++)
	{
		FWeaponInfo WeaponInfo;

		AWeapon* TempWeapon = GameMode->AcquiredWeapons[i];

		WeaponInfo.WeaponType = TempWeapon->GetWeaponType();

		if (TempWeapon == GameMode->EquippedWeapon)
		{
			WeaponInfo.SlotType = ESlotType::EST_Equip;
			bIsArmed = true;
		}
		else if (TempWeapon == GameMode->QuickSlot1Weapon)
		{
			WeaponInfo.SlotType = ESlotType::EST_Quick1;
		}
		else if (TempWeapon == GameMode->QuickSlot2Weapon)
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

	if (GameMode->ItemArray.Num() == 0)
		return;

	for (int32 i = 0; i < GameMode->ItemArray.Num(); i++)
	{
		FItemInfo ItemInfo;

		AItem* TempItem = GameMode->ItemArray[i];

		ItemInfo.ItemType = TempItem->GetItemType();
		
		if (ItemInfo.ItemType == EItemType::EIT_Ammo)
		{
			AItemAmmo* Ammo = Cast<AItemAmmo>(TempItem);
			if (Ammo)
			{
				ItemInfo.ItemDetailType = static_cast<int8>(Ammo->GetAmmoType());
				ItemInfo.ItemCount = GameMode->AmmoQuantityMap[Ammo->GetAmmoType()];
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
				ItemInfo.ItemCount = GameMode->RecoveryItemMap[Recovery->GetRecoveryType()];
				ItemInfo.QuickSlotIndex = Recovery->GetQuickSlotIndex();
				ItemInfo.bIsEquipped = ItemInfo.QuickSlotIndex == GameMode->ActivatedQuickSlotNumber ? true : false;
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