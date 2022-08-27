// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/ItemType.h"
#include "Inventory.generated.h"

/**
 * 
 */

class UVerticalBox;
class UButton;
class UImage;
class UTextBlock;
class UUniformGridPanel;
class UInventorySlot;
class UItemInventorySlot;

USTRUCT()
struct FItemInventoryQuickSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY()
	UImage* ItemIcon;

	UPROPERTY()
	UTextBlock* SlotEquipText;

	UPROPERTY()
	EItemType ItemType;

};

UCLASS()
class ADVENTUREOFSHINBI_API UInventory : public UUserWidget
{
	GENERATED_BODY()

public:

	void BindButtonEvent();

	// 인벤토리 변경 버튼

	UPROPERTY(meta = (BindWidget))
	UButton* InventoryChangeButton;

	// 무기 인벤토리

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* InventoryBox;

	UPROPERTY(meta = (BindWidget))
	UImage* InventoryEquippedWeaponSlotIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* InventoryQuickSlot1Icon;

	UPROPERTY(meta = (BindWidget))
	UImage* InventoryQuickSlot2Icon;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGridPanel;

	TArray<UInventorySlot*> SlotArray;

	// 아이템 인벤토리

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ItemInventoryBox;
	
	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventoryQuickSlot1Icon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot1CountText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventoryQuickSlot2Icon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot2CountText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventoryQuickSlot3Icon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot3CountText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventoryQuickSlot4Icon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot4CountText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventoryQuickSlot5Icon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot5CountText;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* ItemInventoryGridPanel;

	TArray<UItemInventorySlot*> ItemSlotArray;

private:

	UFUNCTION()
	void InventoryChange();

	// 인벤토리 상태 : true , 아이템 인벤토리 상태 : false
	bool bIsInventoryOn = true;
};
