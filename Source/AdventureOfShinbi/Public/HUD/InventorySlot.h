
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/WeaponState.h"
#include "InventorySlot.generated.h"

/**
 * 인벤토리 슬롯 클래스
 */

class UImage;
class AWeapon;
class UButton;
class UVerticalBox;
class UTextBlock;

UCLASS()
class ADVENTUREOFSHINBI_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:

	/** 버튼 델리게이트에 함수 바인딩 */
	void BindSlotClickEvent();

	/** 선택한 메뉴 버튼에 따라 버튼 메뉴 텍스트 변경 */
	void InitializeOthers(EWeaponState State);

	/** 장착 버튼 클릭 시 호출 */
	UFUNCTION()
	void EquipButtonClicked();

	/** 퀵슬롯1 장착 버튼 클릭 시 호출 */
	UFUNCTION()
	void QuickSlot1ButtonClicked();

	/** 퀵슬롯2 장착 버튼 클릭 시 호출 */
	UFUNCTION()
	void QuickSlot2ButtonClicked();

	/** 버리기 버튼 클릭 시 호출 */
	UFUNCTION()
	void DiscardButtonClicked();

protected:

	UFUNCTION()
	void ActivateInventorySlotClick();

	void DeactivateInventorySlotClick();

	void ToggleEquippedSlot();

	void ToggleQuickSlot1();

	void ToggleQuickSlot2();

public:

	UPROPERTY(meta = (BindWidget))
	UImage* InventorySlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* InventorySlotIconButton;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* InventorySlotClick;

	/** 슬롯 메뉴 버튼 및 텍스트 */

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonEquip;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EquipButtonText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuickSlot1ButtonText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuickSlot2ButtonText;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonEquipToQuickSlot1;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonEquipToQuickSlot2;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonDiscardWeapon;

private:

	/** 슬롯에 저장된 무기 아이템 */
	AWeapon* SlottedWeapon;

	/** 무기의 퀵슬롯 저장 여부 */
	bool bIsEquipped = false;
	bool bIsQuickSlot1 = false;
	bool bIsQuickSlot2 = false;

public:

	AWeapon* GetSlottedWeapon() const;
	void SetSlottedWeapon(AWeapon* Weapon);
	void InitializeIcon();
	
};
