
#include "System/AOSGameModeBase.h"
#include "Player/AOSController.h"
#include "Player/AOSPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Weapons/Weapon.h"

AAOSGameModeBase::AAOSGameModeBase()
{
	InitializeTMap();
	InitializeQuickSlotItemArray();
}

void AAOSGameModeBase::RespawnPlayer()
{
	PlayerDeathCount++;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		PlayerPawn->Reset();
		PlayerPawn->Destroy();
	}

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	RestartPlayerAtPlayerStart(GetWorld()->GetFirstPlayerController(), PlayerStarts[0]);
}

bool AAOSGameModeBase::IsPlayerRespawn()
{
	return PlayerDeathCount > 0;
}

void AAOSGameModeBase::AddWeaponToArr(AWeapon* Weapon)
{
	AcquiredWeapons.Add(Weapon);
}

void AAOSGameModeBase::RemoveWeaponFromArr(AWeapon* Weapon)
{
	AcquiredWeapons.Remove(Weapon);
}

void AAOSGameModeBase::KeepEquippedWeapon(AWeapon* Weapon)
{
	EquippedWeapon = Weapon;
}

void AAOSGameModeBase::KeepQuickSlot1Weapon(AWeapon* Weapon)
{
	QuickSlot1Weapon = Weapon;
}

void AAOSGameModeBase::KeepQuickSlot2Weapon(AWeapon* Weapon)
{
	QuickSlot2Weapon = Weapon;
}

int32 AAOSGameModeBase::GetWeaponCount() const
{
	return AcquiredWeapons.Num();
}

AWeapon* AAOSGameModeBase::GetEquippedWeapon()
{
	return EquippedWeapon;
}

AWeapon* AAOSGameModeBase::GetQuickSlot1Weapon()
{
	return QuickSlot1Weapon;
}

AWeapon* AAOSGameModeBase::GetQuickSlot2Weapon()
{
	return QuickSlot2Weapon;
}

void AAOSGameModeBase::InitializeTMap()
{
	RecoveryItemMap.Add(ERecoveryType::ERT_Health, 0);
	RecoveryItemMap.Add(ERecoveryType::ERT_Mana, 0);
	RecoveryItemMap.Add(ERecoveryType::ERT_Stamina, 0);

	AmmoIndexMap.Add(EAmmoType::EAT_AR, -1);
	AmmoIndexMap.Add(EAmmoType::EAT_Sniper, -1);
	AmmoIndexMap.Add(EAmmoType::EAT_Shell, -1);
}

void AAOSGameModeBase::InitializeQuickSlotItemArray()
{
	for (int8 i = 0; i < 5; i++)
	{
		QuickSlotItemArray.Add(FQuickSlotItem{ nullptr, nullptr, nullptr, nullptr });
	}
}

AItem* AAOSGameModeBase::GetItem(int32 Index) const
{
	return ItemArray[Index];
}

void AAOSGameModeBase::AddItemToArr(AItem* Item)
{
	ItemArray.Add(Item);
}

void AAOSGameModeBase::RemoveItemFromArr(AItem* Item)
{
	ItemArray.Remove(Item);
}

int32 AAOSGameModeBase::GetItemIndex(AItem* Item) const
{
	return ItemArray.Find(Item);
}

int32 AAOSGameModeBase::GetItemCount() const
{
	return ItemArray.Num();
}

int32 AAOSGameModeBase::GetRecoveryItemCount(ERecoveryType Type) const
{
	return RecoveryItemMap[Type];
}

void AAOSGameModeBase::AddRecoveryItem(ERecoveryType Type, int32 Quantity)
{
	RecoveryItemMap[Type] += Quantity;
}

void AAOSGameModeBase::AddAmmoToAmmoMap(EAmmoType Type, int32 AmmoQuantity)
{
	AmmoQuantityMap.Add(Type, AmmoQuantity);
}

void AAOSGameModeBase::AddAmmoQuantity(EAmmoType Type, int32 AmmoQuantity)
{
	AmmoQuantityMap[Type] += AmmoQuantity;
}

int32 AAOSGameModeBase::GetAmmoQuantity(EAmmoType Type) const
{
	return AmmoQuantityMap[Type];
}

bool AAOSGameModeBase::IsAmmoTypeExist(EAmmoType Type)
{
	return AmmoQuantityMap.Contains(Type);
}

int32 AAOSGameModeBase::GetAmmoIndex(EAmmoType Type) const
{
	return AmmoIndexMap[Type];
}

void AAOSGameModeBase::SetAmmoIndex(EAmmoType Type, int32 Count)
{
	AmmoIndexMap[Type] = Count;
}

AItem* AAOSGameModeBase::GetQuickSlotItem(int8 Index)
{
	return QuickSlotItemArray[Index].QuickSlotItem;
}

void AAOSGameModeBase::SetQuickSlotItem(int32 Index, AItem* Item, UImage* Icon, UButton* Button, UTextBlock* CountText)
{
	QuickSlotItemArray[Index].QuickSlotItem = Item;
	QuickSlotItemArray[Index].QuickSlotIcon = Icon;
	QuickSlotItemArray[Index].QuickSlotButton = Button;
	QuickSlotItemArray[Index].QuickSlotItemCountText = CountText;
}

void AAOSGameModeBase::SetQuickSlotCountText(int32 Index, FText Text)
{
	QuickSlotItemArray[Index].QuickSlotItemCountText->SetText(Text);
}

void AAOSGameModeBase::ClearQuickSlot(int32 Index)
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
	Brush.SetImageSize(FVector2D(110.f));
	QuickSlotItemArray[Index].QuickSlotIcon->SetBrush(Brush);
	QuickSlotItemArray[Index].QuickSlotItemCountText->SetVisibility(ESlateVisibility::Hidden);
	QuickSlotItemArray[Index] = FQuickSlotItem{ nullptr, nullptr, nullptr, nullptr };
}

int32 AAOSGameModeBase::GetQuickSlotItemArrLength() const
{
	return QuickSlotItemArray.Num();
}

FQuickSlotItem AAOSGameModeBase::GetQuickSlotItemArr(int32 Index)
{
	return QuickSlotItemArray[Index];
}
