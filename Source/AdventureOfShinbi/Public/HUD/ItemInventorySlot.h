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

DECLARE_DELEGATE_OneParam(Delegate_ItemUse, AItem* Item);

UCLASS()
class ADVENTUREOFSHINBI_API UItemInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:

	void BindSlotClickEvent();

	void SetItemSlotCountText(int32 Count);

	void InitializeIcon();

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

	Delegate_ItemUse OnItemUse;

protected:

	UFUNCTION()
	void ActivateItemInventorySlotClick();

	void DeactivateItemInventorySlotClick();

	UFUNCTION()
	void UseButtonClicked();

	UFUNCTION()
	void QuickSlotButtonClicked();

private:

	AItem* SlottedItem;

public:

	void SetSlottedItem(AItem* Item);

};
