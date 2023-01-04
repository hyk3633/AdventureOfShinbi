
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/WeaponState.h"
#include "InventorySlot.generated.h"

/**
 * �κ��丮 ���� Ŭ����
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

	/** ��ư ��������Ʈ�� �Լ� ���ε� */
	void BindSlotClickEvent();

	/** ������ �޴� ��ư�� ���� ��ư �޴� �ؽ�Ʈ ���� */
	void InitializeOthers(EWeaponState State);

	/** ���� ��ư Ŭ�� �� ȣ�� */
	UFUNCTION()
	void EquipButtonClicked();

	/** ������1 ���� ��ư Ŭ�� �� ȣ�� */
	UFUNCTION()
	void QuickSlot1ButtonClicked();

	/** ������2 ���� ��ư Ŭ�� �� ȣ�� */
	UFUNCTION()
	void QuickSlot2ButtonClicked();

	/** ������ ��ư Ŭ�� �� ȣ�� */
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

	/** ���� �޴� ��ư �� �ؽ�Ʈ */

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

	/** ���Կ� ����� ���� ������ */
	AWeapon* SlottedWeapon;

	/** ������ ������ ���� ���� */
	bool bIsEquipped = false;
	bool bIsQuickSlot1 = false;
	bool bIsQuickSlot2 = false;

public:

	AWeapon* GetSlottedWeapon() const;
	void SetSlottedWeapon(AWeapon* Weapon);
	void InitializeIcon();
	
};
