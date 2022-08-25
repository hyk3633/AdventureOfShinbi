// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/WeaponState.h"
#include "InventorySlot.generated.h"

/**
 * 
 */

class UImage;
class AWeapon;
class UButton;
class AAOSHUD;
class UVerticalBox;
class UTextBlock;

UCLASS()
class ADVENTUREOFSHINBI_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:

	void BindSlotClickEvent();

	void InitializeOthers(EWeaponState State);

	UPROPERTY(meta = (BindWidget))
	UImage* InventorySlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* InventorySlotIconButton;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* InventorySlotClick;

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

	UFUNCTION()
	void EquipButtonClicked();

	UFUNCTION()
	void QuickSlot1ButtonClicked();

	UFUNCTION()
	void QuickSlot2ButtonClicked();

	UFUNCTION()
	void DiscardButtonClicked();

protected:

	UFUNCTION()
	void ActivateInventorySlotClick();

	void DeactivateInventorySlotClick();

	void ToggleEquippedSlot();

	void ToggleQuickSlot1();

	void ToggleQuickSlot2();

private:

	AWeapon* SlottedWeapon;

	bool bIsEquipped = false;
	bool bIsQuickSlot1 = false;
	bool bIsQuickSlot2 = false;

public:

	AWeapon* GetSlottedWeapon() const;
	void SetSlottedWeapon(AWeapon* Weapon);
	void InitializeIcon();
	
};
