
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/ItemType.h"
#include "ItemInventorySlot.generated.h"

/**
 * ������ �κ��丮 ���� Ŭ����
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

	/** ��ư ��������Ʈ�� �Լ� ���ε� */
	void BindSlotClickEvent();

	/** ������ ���� �ؽ�Ʈ ���� */
	void SetItemSlotCountText(int32 Count);

	/** ������ �ʱ�ȭ */
	void InitializeIcon();

	/** �����Կ� ��� ��ư Ŭ�� �� ȣ�� */
	UFUNCTION()
	void QuickSlotButtonClicked();

protected:

	/** ������ ���� ��ư Ŭ���� ȣ�� */
	UFUNCTION()
	void ActivateItemInventorySlotClick();

	/** ��� ��ư Ŭ�� �� ȣ�� */
	UFUNCTION()
	void UseButtonClicked();

	/** ��� ��ư Ŭ�� �� ȣ�� */
	UFUNCTION()
	void CancelButtonClicked();

public:

	UPROPERTY(meta = (BindWidget))
	UImage* ItemInventorySlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemInventorySlotIconButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemSlotCountText;

	/** ���� �޴� ��ư �� �ؽ�Ʈ */

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

	/** ���Կ� ����� ������ */
	AItem* SlottedItem;

	/** �������� �����Կ� �����Ǿ� ������ true */
	bool bItemEquippedQuickSlot = false;

public:

	void DeactivateItemInventorySlotClick();
	void SetSlottedItem(AItem* Item);
	void SetItemEquippedQuickSlot(const bool IsEquipped);

};
