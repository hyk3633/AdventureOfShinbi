// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InventorySlot.h"
#include "HUD/AOSHUD.h"
#include "HUD/InventorySlotClick.h"
#include "Components/Button.h"

void UInventorySlot::BindSlotClickEvent()
{
	if (InventorySlotIconButton)
	{
		InventorySlotIconButton->OnClicked.AddDynamic(this, &UInventorySlot::ActivateInventorySlotClick);
	}
	if (InventorySlotClick)
	{
		//InventorySlotClick->ButtonEquip->OnClicked.AddDynamic(this, );
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
