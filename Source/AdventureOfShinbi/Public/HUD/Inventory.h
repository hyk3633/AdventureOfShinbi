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

DECLARE_DELEGATE_ThreeParams(Delegate_ItemQuickSelect, int8 SlotIndex, UImage* QuickSlotIcon, UTextBlock* QuickSlotCountText);

UCLASS()
class ADVENTUREOFSHINBI_API UInventory : public UUserWidget
{
	GENERATED_BODY()

public:

	void BindButtonEvent();

	void AllQuickSlotButtonEnabled();

	void AllQuickSlotButtonDisabled();

	UFUNCTION()
	void QuickSlot1Clicked();
	UFUNCTION()
	void QuickSlot2Clicked();
	UFUNCTION()
	void QuickSlot3Clicked();
	UFUNCTION()
	void QuickSlot4Clicked();
	UFUNCTION()
	void QuickSlot5Clicked();

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
	UButton* ItemInventoryQuickSlot1Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot1CountText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventoryQuickSlot2Icon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemInventoryQuickSlot2Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot2CountText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventoryQuickSlot3Icon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemInventoryQuickSlot3Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot3CountText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventoryQuickSlot4Icon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemInventoryQuickSlot4Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot4CountText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventoryQuickSlot5Icon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemInventoryQuickSlot5Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInventoryQuickSlot5CountText;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* ItemInventoryGridPanel;

	TArray<UItemInventorySlot*> ItemSlotArray;

	// 위젯 애니메이션

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Highlighting;

	Delegate_ItemQuickSelect OnItemQuickSlotSelected;

private:

	UFUNCTION()
	void InventoryChange();

	// 인벤토리 상태 : true , 아이템 인벤토리 상태 : false
	bool bIsInventoryOn = true;
};
