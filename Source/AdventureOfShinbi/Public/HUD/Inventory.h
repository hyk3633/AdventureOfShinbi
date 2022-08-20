// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory.generated.h"

/**
 * 
 */

class UImage;
class UUniformGridPanel;
class UUniformGridSlot;
class UInventorySlot;

UCLASS()
class ADVENTUREOFSHINBI_API UInventory : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UImage* InventoryEquippedWeaponSlotIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* InventoryQuickSlot1Icon;

	UPROPERTY(meta = (BindWidget))
	UImage* InventoryQuickSlot2Icon;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGridPanel;

	TArray<UInventorySlot*> SlotArray;
	
};
