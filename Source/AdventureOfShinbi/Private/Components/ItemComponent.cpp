﻿

#include "Components/ItemComponent.h"
#include "Components/CombatComponent.h"
#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "HUD/AOSHUD.h"
#include "HUD/AOSCharacterOverlay.h"
#include "HUD/Inventory.h"
#include "HUD/ItemInventorySlot.h"
#include "Items/Item.h"
#include "Items/ItemRecovery.h"
#include "Items/ItemAmmo.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeTMap();

	InitializeQuickSlotItemArray();

	CharacterController = Cast<AAOSController>(Character->GetController());
	if (CharacterController)
	{
		HUD = Cast<AAOSHUD>(CharacterController->GetHUD());
	}

	HUD->CharacterOverlay->InventoryWidget->OnItemQuickSlotSelected.BindUObject(this, &UItemComponent::EquipToItemQuickSlot);
}

void UItemComponent::AddItem(AItem* Item)
{
	if (ItemArray.Num() % 5 == 0)
	{
		HUD->CreateItemInventorySlot();
	}

	switch (Item->GetItemType())
	{
	case EItemType::EIT_Recovery:
		AddRecoveryItem(Item);
		break;
	case EItemType::EIT_Ammo:
		AddAmmoItem(Item);
		break;
	}
}

void UItemComponent::UseActivatedQuickSlotItem()
{
	if (QuickSlotItemArray[ActivatedQuickSlotNumber].QuickSlotItem)
	{
		UseItem(QuickSlotItemArray[ActivatedQuickSlotNumber].QuickSlotItem);
	}
}

void UItemComponent::ItemChange()
{
	ActivatedQuickSlotNumber = (ActivatedQuickSlotNumber + 1) % 5;

	for (int8 i = 0; i < 4; i++)
	{
		if (QuickSlotItemArray[ActivatedQuickSlotNumber].QuickSlotItem)
		{
			FSlateBrush Brush;
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.SetImageSize(FVector2D(110.f));
			HUD->CharacterOverlay->EquippedItemIcon->SetBrush(Brush);
			HUD->CharacterOverlay->EquippedItemIcon->SetBrushFromTexture(QuickSlotItemArray[ActivatedQuickSlotNumber].QuickSlotItem->GetItemIcon());
			HUD->CharacterOverlay->EquippedItemSlotCountText->SetText(FText::FromString(FString::FromInt(GetItemCount(QuickSlotItemArray[ActivatedQuickSlotNumber].QuickSlotItem))));
			HUD->CharacterOverlay->EquippedItemSlotCountText->SetVisibility(ESlateVisibility::Visible);

			return;
		}
		else
		{
			ActivatedQuickSlotNumber = (ActivatedQuickSlotNumber + 1) % 5;
		}
	}
}

void UItemComponent::AddRecoveryItem(AItem* Item)
{
	AItemRecovery* Recovery = Cast<AItemRecovery>(Item);

	if (RecoveryItemMap[Recovery->GetRecoveryType()] == 0)
	{
		ItemArray.Add(Item);
		HUD->AddItemToSlot(ItemArray.Num()-1,Item);
		HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num()-1]->OnItemMenuSelect.BindUObject(this, &UItemComponent::ItemUseOrEquip);
		Recovery->GetStaticMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Recovery->GetStaticMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Recovery->GetStaticMesh()->SetVisibility(false);
	}
	else
	{
		Item->Destroy();
	}

	RecoveryItemMap[Recovery->GetRecoveryType()] += 1;

	HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num()-1]->SetItemSlotCountText(RecoveryItemMap[Recovery->GetRecoveryType()]);
}

void UItemComponent::AddAmmoItem(AItem* Item)
{
	AItemAmmo* Ammo = Cast<AItemAmmo>(Item);

	if (AmmoQuantityMap[Ammo->GetAmmoType()] == 0)
	{
		ItemArray.Add(Item);
		HUD->AddItemToSlot(ItemArray.Num()-1, Item);
		HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num() - 1]->ItemInventorySlotIconButton->SetIsEnabled(false);
		HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num()-1]->OnItemMenuSelect.BindUObject(this, &UItemComponent::ItemUseOrEquip);
		Ammo->GetStaticMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Ammo->GetStaticMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Ammo->GetStaticMesh()->SetVisibility(false);
	}
	else
	{
		Item->Destroy();
	}

	AmmoQuantityMap[Ammo->GetAmmoType()] += Ammo->GetAmmoQuantity();

	HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num()-1]->SetItemSlotCountText(AmmoQuantityMap[Ammo->GetAmmoType()]);

}

void UItemComponent::ItemUseOrEquip(AItem* Item, EItemSlotMenuState State)
{
	if (State == EItemSlotMenuState::EISMS_Use)
	{
		UseItem(Item);
	}
	else if(State == EItemSlotMenuState::EISMS_Equip)
	{
		SelectedItem = Item;
		HUD->CharacterOverlay->InventoryWidget->AllQuickSlotButtonEnabled();
		Character->GetWorldTimerManager().SetTimer(QuickSlotAnimationTimer, this, &UItemComponent::PlayQuickSlotAnimation, QuickSlotAnimationTime, true, 0.f);
	}
	else if (State == EItemSlotMenuState::EISMS_Cancel)
	{
		HUD->CharacterOverlay->InventoryWidget->AllQuickSlotButtonDisabled();
		Character->GetWorldTimerManager().ClearTimer(QuickSlotAnimationTimer);
	}
	else if (State == EItemSlotMenuState::EISMS_Dismount)
	{
		DismountItem(Item);
	}
}

void UItemComponent::UseItem(AItem* Item)
{
	switch (Item->GetItemType())
	{
	case EItemType::EIT_Recovery:
		UseRecoveryItem(Item);
		break;
	case EItemType::EIT_Ammo:
		AddAmmoItem(Item);
		break;
	}
}

void UItemComponent::EquipToItemQuickSlot(int8 SlotIndex, UImage* QuickSlotIcon, UButton* QuickSlotButton, UTextBlock* QuickSlotCountText)
{
	HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(SelectedItem)]->ButtonCancel->SetVisibility(ESlateVisibility::Hidden);
	HUD->CharacterOverlay->InventoryWidget->AllQuickSlotButtonDisabled();
	Character->GetWorldTimerManager().ClearTimer(QuickSlotAnimationTimer);

	if (SelectedItem)
	{
		if (QuickSlotIcon && QuickSlotButton && QuickSlotCountText)
		{
			// 선택한 아이템이 퀵슬롯에 이미 등록되어 있을 경우
			if (QuickSlotItemArray[SlotIndex].QuickSlotItem == SelectedItem)
			{
				SelectedItem = nullptr;
				return;
			}
			// 퀵슬롯이 이미 차 있을 경우
			else if (QuickSlotItemArray[SlotIndex].QuickSlotItem != nullptr)
			{
				// TODO : 먼저 장착되어 있던 아이템의 아이템 슬롯의 퀵슬롯 장착 버튼의 텍스트를 '퀵슬롯에 장착'으로 바꾸기
				HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(QuickSlotItemArray[SlotIndex].QuickSlotItem)]->
					ItemQuickSlotButtonText->SetText(FText::FromString(TEXT("퀵슬롯에 장착")));
				HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(QuickSlotItemArray[SlotIndex].QuickSlotItem)]->SetItemEquippedQuickSlot(false);
			}

			if (CheckQuickSlotArrayIsEmpty())
			{
				ActivatedQuickSlotNumber = SlotIndex;
				FSlateBrush Brush;
				Brush.DrawAs = ESlateBrushDrawType::Image;
				Brush.SetImageSize(FVector2D(110.f));
				HUD->CharacterOverlay->EquippedItemIcon->SetBrush(Brush);
				HUD->CharacterOverlay->EquippedItemIcon->SetBrushFromTexture(SelectedItem->GetItemIcon());
				HUD->CharacterOverlay->EquippedItemSlotCountText->SetText(FText::FromString(FString::FromInt(GetItemCount(SelectedItem))));
				HUD->CharacterOverlay->EquippedItemSlotCountText->SetVisibility(ESlateVisibility::Visible);
			}

			QuickSlotItemArray[SlotIndex].QuickSlotItem = SelectedItem;
			QuickSlotItemArray[SlotIndex].QuickSlotIcon = QuickSlotIcon;
			QuickSlotItemArray[SlotIndex].QuickSlotButton = QuickSlotButton;
			QuickSlotItemArray[SlotIndex].QuickSlotItemCountText = QuickSlotCountText;

			FSlateBrush Brush;
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.SetImageSize(FVector2D(110.f));
			QuickSlotIcon->SetBrush(Brush);
			QuickSlotIcon->SetBrushFromTexture(SelectedItem->GetItemIcon());
			QuickSlotCountText->SetText(FText::FromString(FString::FromInt(GetItemCount(SelectedItem))));
			QuickSlotCountText->SetVisibility(ESlateVisibility::Visible);

			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(SelectedItem)]->ButtonEquipToQuickSlot->SetIsEnabled(true);
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(SelectedItem)]->ButtonCancel->SetVisibility(ESlateVisibility::Hidden);
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(SelectedItem)]->DeactivateItemInventorySlotClick();
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(SelectedItem)]->SetItemEquippedQuickSlot(true);
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(SelectedItem)]->
				ItemQuickSlotButtonText->SetText(FText::FromString(TEXT("퀵슬롯 장착 해제")));
			SelectedItem = nullptr;
		}
	}
}

bool UItemComponent::CheckQuickSlotArrayIsEmpty()
{
	for (int8 i = 0; i < 5; i++)
	{
		if (QuickSlotItemArray[i].QuickSlotItem != nullptr)
		{
			return false;
		}
	}

	return true;
}

void UItemComponent::DismountItem(AItem* Item)
{
	HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(Item)]->
		ItemQuickSlotButtonText->SetText(FText::FromString(TEXT("퀵슬롯에 장착")));
	HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(Item)]->SetItemEquippedQuickSlot(false);
	
	for (int8 i = 0; i < 5; i++)
	{
		if (QuickSlotItemArray[i].QuickSlotItem == Item)
		{
			FSlateBrush Brush;
			Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
			Brush.SetImageSize(FVector2D(110.f));
			QuickSlotItemArray[i].QuickSlotIcon->SetBrush(Brush);

			QuickSlotItemArray[i].QuickSlotItemCountText->SetVisibility(ESlateVisibility::Hidden);

			QuickSlotItemArray[i] = FQuickSlotItem{ nullptr, nullptr, nullptr, nullptr };

			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(Item)]->DeactivateItemInventorySlotClick();

			if (ActivatedQuickSlotNumber == i)
			{
				HUD->CharacterOverlay->EquippedItemIcon->SetBrush(Brush);
				HUD->CharacterOverlay->EquippedItemSlotCountText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UItemComponent::PlayQuickSlotAnimation()
{
	HUD->CharacterOverlay->InventoryWidget->PlayAnimation(
		HUD->CharacterOverlay->InventoryWidget->Highlighting,
		0.f,
		1,
		EUMGSequencePlayMode::PingPong,
		1.f,
		true);
}

void UItemComponent::UseRecoveryItem(AItem* Item)
{
	AItemRecovery* IR = Cast<AItemRecovery>(Item);
	if(IR)
	{
		if (RecoveryItemMap[IR->GetRecoveryType()] > 0)
		{
			RecoveryItemMap[IR->GetRecoveryType()] -= 1;
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(Item)]->SetItemSlotCountText(RecoveryItemMap[IR->GetRecoveryType()]);

			for (int8 i = 0; i < 5; i++)
			{
				if (QuickSlotItemArray[i].QuickSlotItem == Item)
				{
					QuickSlotItemArray[i].QuickSlotItemCountText->SetText(FText::FromString(FString::FromInt(RecoveryItemMap[IR->GetRecoveryType()])));

					if (ActivatedQuickSlotNumber == i)
					{
						HUD->CharacterOverlay->EquippedItemSlotCountText->SetText(FText::FromString(FString::FromInt(RecoveryItemMap[IR->GetRecoveryType()])));
					}
				}
			}

			if (IR->GetRecoveryType() == ERecoveryType::ERT_Health)
			{
				bDoRecoveryHealth = true;
			}
			else if (IR->GetRecoveryType() == ERecoveryType::ERT_Mana)
			{
				bDoRecoveryMana = true;
			}
			else if (IR->GetRecoveryType() == ERecoveryType::ERT_Stamina)
			{

			}
		}

		if (RecoveryItemMap[IR->GetRecoveryType()] == 0)
		{
			// 만일 퀵슬롯에 아이템이 장착되어 있으면 퀵슬롯 비우기
			DismountItem(Item);

			UItemInventorySlot* Slot = HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(Item)];
			Slot->InitializeIcon();
			Slot->SetSlottedItem(nullptr);
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray.Remove(Slot);
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray.Add(Slot);
			HUD->UpdateItemInventory();
			ItemArray.Remove(Item);
		}
	}
}

void UItemComponent::Recovery(float DeltaTime)
{
	if (bDoRecoveryHealth)
	{
		float Increase = HealthRecoveryRate * DeltaTime;
		RecoveredHealthAmount += Increase;
		if (RecoveredHealthAmount <= HealthRecoveryAmount)
		{
			CombatComp->Health = FMath::Clamp(CombatComp->Health + Increase, 0.f, CombatComp->MaxHealth);
			CharacterController->SetHUDHealthBar(CombatComp->Health, CombatComp->MaxHealth);
		}
		else
		{
			bDoRecoveryHealth = false;
			RecoveredHealthAmount = 0.f;
		}
	}
	if (bDoRecoveryMana)
	{
		float Increase = ManaRecoveryRate * DeltaTime;
		RecoveredManaAmount += Increase;
		if (RecoveredManaAmount <= ManaRecoveryAmount)
		{
			CombatComp->Mana = FMath::Clamp(CombatComp->Mana + Increase, 0.f, CombatComp->MaxMana);
			CharacterController->SetHUDManaBar(CombatComp->Mana, CombatComp->MaxMana);
		}
		else
		{
			bDoRecoveryMana = false;
			RecoveredManaAmount = 0.f;
		}
	}
}

int32 UItemComponent::GetItemCount(AItem* Item)
{
	if (Item->GetItemType() == EItemType::EIT_Recovery)
	{
		AItemRecovery* IR = Cast<AItemRecovery>(Item);
		return RecoveryItemMap[IR->GetRecoveryType()];
	}
	else if (Item->GetItemType() == EItemType::EIT_Ammo)
	{
		AItemAmmo* IA = Cast<AItemAmmo>(Item);
		return AmmoQuantityMap[IA->GetAmmoType()];
	}
	else
	{
		return 0;
	}
}

void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Recovery(DeltaTime);
}

void UItemComponent::InitializeTMap()
{
	RecoveryItemMap.Add(ERecoveryType::ERT_Health, 0);
	RecoveryItemMap.Add(ERecoveryType::ERT_Mana, 0);
	RecoveryItemMap.Add(ERecoveryType::ERT_Stamina, 0);

	AmmoQuantityMap.Add(EAmmoType::EAT_AR, 0);
	AmmoQuantityMap.Add(EAmmoType::EAT_SMG, 20);
	AmmoQuantityMap.Add(EAmmoType::EAT_Pistol, 0);
	AmmoQuantityMap.Add(EAmmoType::EAT_Shell, 0);
	AmmoQuantityMap.Add(EAmmoType::EAT_GrenadeLauncher, 0);
	AmmoQuantityMap.Add(EAmmoType::EAT_Rocket, 0);
}

void UItemComponent::InitializeQuickSlotItemArray()
{
	for (int8 i = 0; i < 5; i++)
	{
		QuickSlotItemArray.Add(FQuickSlotItem{ nullptr, nullptr, nullptr, nullptr });
	}
}

void UItemComponent::SetCharacter(AAOSCharacter* CharacterToSet)
{
	Character = CharacterToSet;
}

void UItemComponent::SetCombatComp(UCombatComponent* Combat)
{
	CombatComp = Combat;
}

TMap<EAmmoType, int32> UItemComponent::GetAmmoMap() const
{
	return AmmoQuantityMap;
}
