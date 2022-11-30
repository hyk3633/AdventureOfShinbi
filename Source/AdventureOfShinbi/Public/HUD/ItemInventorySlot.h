// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/ItemType.h"
#include "ItemInventorySlot.generated.h"

/**
 * 
 */

class AItem;
class UImage;
class UButton;
class AAOSHUD;
class UVerticalBox;
class UTextBlock;

DECLARE_DELEGATE_TwoParams(Delegate_ItemMenuSelect, AItem* Item, EItemSlotMenuState State);

UCLASS()
class ADVENTUREOFSHINBI_API UItemInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:

	void BindSlotClickEvent();

	void SetItemSlotCountText(int32 Count);

	void InitializeIcon();

	UFUNCTION()
	void QuickSlotButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventorySlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemInventorySlotIconButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemSlotCountText;

	// ½½·Ô ¸Þ´º

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ItemInventorySlotClick;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonUse;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonEquipToQuickSlot;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemQuickSlotButtonText;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonCancel;

	Delegate_ItemMenuSelect OnItemMenuSelect;

protected:

	UFUNCTION()
	void ActivateItemInventorySlotClick();

	UFUNCTION()
	void UseButtonClicked();

	UFUNCTION()
	void CancelButtonClicked();

private:

	AItem* SlottedItem;

	bool bItemEquippedQuickSlot = false;

public:

	void DeactivateItemInventorySlotClick();
	void SetSlottedItem(AItem* Item);
	void SetItemEquippedQuickSlot(const bool IsEquipped);
};
