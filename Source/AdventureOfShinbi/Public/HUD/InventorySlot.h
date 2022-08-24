// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlot.generated.h"

/**
 * 
 */

class UImage;
class AWeapon;
class UButton;
class AAOSHUD;
class UInventorySlotClick;

UCLASS()
class ADVENTUREOFSHINBI_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UImage* InventorySlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* InventorySlotIconButton;

	UPROPERTY(meta = (BindWidget))
	UInventorySlotClick* InventorySlotClick;

	AWeapon* GetSlottedWeapon() const { return SlottedWeapon; }
	void SetSlottedWeapon(AWeapon* Weapon) { SlottedWeapon = Weapon; }

	void BindSlotClickEvent();

protected:

	UFUNCTION()
	void ActivateInventorySlotClick();

private:

	AWeapon* SlottedWeapon;

	//UPROPERTY(Transient, meta = (BindWidgetAnim))
	//UWidgetAnimation* AnimInventorySlotClick;
	
};
