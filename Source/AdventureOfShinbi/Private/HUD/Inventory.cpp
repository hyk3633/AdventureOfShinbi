// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Inventory.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"

void UInventory::BindButtonEvent()
{
	if (InventoryChangeButton)
	{
		InventoryChangeButton->OnClicked.AddDynamic(this, &UInventory::InventoryChange);
	}

	if (ItemInventoryQuickSlot1Button)
	{
		ItemInventoryQuickSlot1Button->OnClicked.AddDynamic(this, &UInventory::QuickSlot1Clicked);
	}
	if (ItemInventoryQuickSlot2Button)
	{						  
		ItemInventoryQuickSlot2Button->OnClicked.AddDynamic(this, &UInventory::QuickSlot2Clicked);
	}
	if (ItemInventoryQuickSlot3Button)
	{
		ItemInventoryQuickSlot3Button->OnClicked.AddDynamic(this, &UInventory::QuickSlot3Clicked);
	}
	if (ItemInventoryQuickSlot4Button)
	{
		ItemInventoryQuickSlot4Button->OnClicked.AddDynamic(this, &UInventory::QuickSlot4Clicked);
	}
	if (ItemInventoryQuickSlot5Button)
	{
		ItemInventoryQuickSlot5Button->OnClicked.AddDynamic(this, &UInventory::QuickSlot5Clicked);
	}
}

void UInventory::AllQuickSlotButtonEnabled()
{
	ItemInventoryQuickSlot1Button->SetIsEnabled(true);
	ItemInventoryQuickSlot2Button->SetIsEnabled(true);
	ItemInventoryQuickSlot3Button->SetIsEnabled(true);
	ItemInventoryQuickSlot4Button->SetIsEnabled(true);
	ItemInventoryQuickSlot5Button->SetIsEnabled(true);
}

void UInventory::AllQuickSlotButtonDisabled()
{
	ItemInventoryQuickSlot1Button->SetIsEnabled(false);
	ItemInventoryQuickSlot2Button->SetIsEnabled(false);
	ItemInventoryQuickSlot3Button->SetIsEnabled(false);
	ItemInventoryQuickSlot4Button->SetIsEnabled(false);
	ItemInventoryQuickSlot5Button->SetIsEnabled(false);
}

void UInventory::QuickSlot1Clicked()
{
	OnItemQuickSlotSelected.ExecuteIfBound(0, ItemInventoryQuickSlot1Icon, ItemInventoryQuickSlot1CountText);
}

void UInventory::QuickSlot2Clicked()
{
	OnItemQuickSlotSelected.ExecuteIfBound(1, ItemInventoryQuickSlot2Icon, ItemInventoryQuickSlot2CountText);
}

void UInventory::QuickSlot3Clicked()
{
	OnItemQuickSlotSelected.ExecuteIfBound(2, ItemInventoryQuickSlot3Icon, ItemInventoryQuickSlot3CountText);
}

void UInventory::QuickSlot4Clicked()
{
	OnItemQuickSlotSelected.ExecuteIfBound(3, ItemInventoryQuickSlot4Icon, ItemInventoryQuickSlot4CountText);
}

void UInventory::QuickSlot5Clicked()
{
	OnItemQuickSlotSelected.ExecuteIfBound(4, ItemInventoryQuickSlot5Icon, ItemInventoryQuickSlot5CountText);
}

void UInventory::InventoryChange()
{
	if (bIsInventoryOn)
	{
		bIsInventoryOn = false;

		if (InventoryBox)
		{
			InventoryBox->SetIsEnabled(false);
			InventoryBox->SetVisibility(ESlateVisibility::Hidden);
		}
		if (ItemInventoryBox)
		{
			ItemInventoryBox->SetIsEnabled(true);
			ItemInventoryBox->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		bIsInventoryOn = true;

		if (InventoryBox)
		{
			InventoryBox->SetIsEnabled(true);
			InventoryBox->SetVisibility(ESlateVisibility::Visible);
		}
		if (ItemInventoryBox)
		{
			ItemInventoryBox->SetIsEnabled(false);
			ItemInventoryBox->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}