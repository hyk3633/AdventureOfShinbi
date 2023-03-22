// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/ItemType.h"
#include "Inventory.generated.h"

/**
 * 인벤토리 클래스
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

	/** 버튼 이벤트 바인딩 */
	void BindButtonEvent();

	/** 모든 아이템 퀵슬롯 버튼 활성화 */
	void AllQuickSlotButtonEnabled();

	/** 모든 아이템 퀵슬롯 버튼 비활성화 */
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

protected:

	/** bIsInventoryOn에 따라 무기/아이템 인벤토리 활성화 및 비활성화 처리 함수 */
	UFUNCTION()
	void InventoryChange();

public:

	UPROPERTY(meta = (BindWidget))
	UButton* InventoryChangeButton;

	/** 무기 인벤토리 */

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

	UPROPERTY()
	TArray<UInventorySlot*> SlotArray;

	/** 아이템 인벤토리 */

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

	UPROPERTY()
	TArray<UItemInventorySlot*> ItemSlotArray;

	/** 위젯 애니메이션 */

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Highlighting;

	Delegate_ItemQuickSelect OnItemQuickSlotSelected;

private:

	/** true 이면 무기 인벤토리, false 이면 아이템 인벤토리 활성화 */
	bool bIsInventoryOn = true;
};
