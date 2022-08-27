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