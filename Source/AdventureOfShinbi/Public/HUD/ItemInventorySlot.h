
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/ItemType.h"
#include "ItemInventorySlot.generated.h"

/**
 * 아이템 인벤토리 슬롯 클래스
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

	/** 버튼 델리게이트에 함수 바인딩 */
	void BindSlotClickEvent();

	/** 아이템 슬롯 텍스트 설정 */
	void SetItemSlotCountText(int32 Count);

	/** 아이콘 초기화 */
	void InitializeIcon();

	/** 퀵슬롯에 등록 버튼 클릭 시 호출 */
	UFUNCTION()
	void QuickSlotButtonClicked();

protected:

	/** 아이템 슬롯 버튼 클릭시 호출 */
	UFUNCTION()
	void ActivateItemInventorySlotClick();

	/** 사용 버튼 클릭 시 호출 */
	UFUNCTION()
	void UseButtonClicked();

	/** 취소 버튼 클릭 시 호출 */
	UFUNCTION()
	void CancelButtonClicked();

public:

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventorySlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemInventorySlotIconButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemSlotCountText;

	/** 슬롯 메뉴 버튼 및 텍스트 */

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

private:

	/** 슬롯에 저장된 아이템 */
	AItem* SlottedItem;

	/** 아이템이 퀵슬롯에 장착되어 있으면 true */
	bool bItemEquippedQuickSlot = false;

public:

	void DeactivateItemInventorySlotClick();
	void SetSlottedItem(AItem* Item);
	void SetItemEquippedQuickSlot(const bool IsEquipped);

};
