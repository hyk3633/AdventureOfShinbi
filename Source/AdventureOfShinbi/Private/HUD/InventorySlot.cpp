// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InventorySlot.h"
#include "HUD/AOSHUD.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Weapons/Weapon.h"
#include "Weapons/MeleeWeapon.h"

void UInventorySlot::BindSlotClickEvent()
{
	if (InventorySlotIconButton)
	{
		InventorySlotIconButton->OnClicked.AddDynamic(this, &UInventorySlot::ActivateInventorySlotClick);
	}
	if (InventorySlotClick && SlottedWeapon)
	{
		if (ButtonEquip)
		{
			ButtonEquip->OnClicked.AddDynamic(this, &UInventorySlot::EquipButtonClicked);
		}
		if (ButtonEquipToQuickSlot1)
		{
			ButtonEquipToQuickSlot1->OnClicked.AddDynamic(this, &UInventorySlot::QuickSlot1ButtonClicked);
		}
		if (ButtonEquipToQuickSlot2)
		{
			ButtonEquipToQuickSlot2->OnClicked.AddDynamic(this, &UInventorySlot::QuickSlot2ButtonClicked);
		}
		if (ButtonDiscardWeapon)
		{
			ButtonDiscardWeapon->OnClicked.AddDynamic(this, &UInventorySlot::DiscardButtonClicked);
		}
	}
}

void UInventorySlot::ToggleEquippedSlot()
{
	if (bIsEquipped)
	{
		bIsEquipped = false;
		SlottedWeapon->SetWeaponState(EWeaponState::EWS_PickedUp);
		EquipButtonText->SetText(FText::FromString(TEXT("장착하기")));
	}
	else
	{
		bIsEquipped = true;
		SlottedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		EquipButtonText->SetText(FText::FromString(TEXT("장착 해제")));
	}
}

void UInventorySlot::ToggleQuickSlot1()
{
	if (bIsQuickSlot1)
	{
		bIsQuickSlot1 = false;
		SlottedWeapon->SetWeaponState(EWeaponState::EWS_PickedUp);
		QuickSlot1ButtonText->SetText(FText::FromString(TEXT("퀵슬롯1에 장착")));
	}
	else
	{
		bIsQuickSlot1 = true;
		SlottedWeapon->SetWeaponState(EWeaponState::EWS_QuickSlot1);
		QuickSlot1ButtonText->SetText(FText::FromString(TEXT("퀵슬롯1 해제")));
	}

}

void UInventorySlot::ToggleQuickSlot2()
{
	if (bIsQuickSlot2)
	{
		bIsQuickSlot2 = false;
		SlottedWeapon->SetWeaponState(EWeaponState::EWS_PickedUp);
		QuickSlot2ButtonText->SetText(FText::FromString(TEXT("퀵슬롯2에 장착")));
	}
	else
	{
		bIsQuickSlot2 = true;
		SlottedWeapon->SetWeaponState(EWeaponState::EWS_QuickSlot2);
		QuickSlot2ButtonText->SetText(FText::FromString(TEXT("퀵슬롯2 해제")));
	}
}

void UInventorySlot::InitializeOthers(EWeaponState State)
{
	if (State == EWeaponState::EWS_Equipped)
	{
		bIsQuickSlot1 = false;
		bIsQuickSlot2 = false;
		QuickSlot1ButtonText->SetText(FText::FromString(TEXT("퀵슬롯1에 장착")));
		QuickSlot2ButtonText->SetText(FText::FromString(TEXT("퀵슬롯2에 장착")));
	}
	else if (State == EWeaponState::EWS_QuickSlot1)
	{
		bIsEquipped = false;
		bIsQuickSlot2 = false;
		EquipButtonText->SetText(FText::FromString(TEXT("장착하기")));
		QuickSlot2ButtonText->SetText(FText::FromString(TEXT("퀵슬롯2에 장착")));
	}
	else if (State == EWeaponState::EWS_QuickSlot2)
	{
		bIsEquipped = false;
		bIsQuickSlot1 = false;
		EquipButtonText->SetText(FText::FromString(TEXT("장착하기")));
		QuickSlot1ButtonText->SetText(FText::FromString(TEXT("퀵슬롯1에 장착")));
	}
}

void UInventorySlot::ActivateInventorySlotClick()
{
	if (InventorySlotClick)
	{
		InventorySlotClick->SetIsEnabled(true);
		InventorySlotClick->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInventorySlot::DeactivateInventorySlotClick()
{
	if (InventorySlotClick)
	{
		InventorySlotClick->SetIsEnabled(false);
		InventorySlotClick->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventorySlot::EquipButtonClicked()
{
	if (SlottedWeapon)
	{
		InitializeOthers(EWeaponState::EWS_Equipped);
		ToggleEquippedSlot();
		DeactivateInventorySlotClick();
	}
}

void UInventorySlot::QuickSlot1ButtonClicked()
{
	if (SlottedWeapon)
	{
		InitializeOthers(EWeaponState::EWS_QuickSlot1);
		ToggleQuickSlot1();
		DeactivateInventorySlotClick();
	}
}

void UInventorySlot::QuickSlot2ButtonClicked()
{
	if (SlottedWeapon)
	{
		InitializeOthers(EWeaponState::EWS_QuickSlot2);
		ToggleQuickSlot2();
		DeactivateInventorySlotClick();
	}
}

void UInventorySlot::DiscardButtonClicked()
{
	if (SlottedWeapon)
	{
		SlottedWeapon->SetWeaponState(EWeaponState::EWS_Field);
		DeactivateInventorySlotClick();
	}
}

AWeapon* UInventorySlot::GetSlottedWeapon() const 
{ 
	return SlottedWeapon; 
}

void UInventorySlot::SetSlottedWeapon(AWeapon* Weapon) 
{ 
	SlottedWeapon = Weapon; 
}

void UInventorySlot::InitializeIcon()
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
	InventorySlotIcon->SetBrush(Brush);
}
