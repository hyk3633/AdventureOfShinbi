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

UCLASS()
class ADVENTUREOFSHINBI_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UImage* InventorySlotIcon;

	AWeapon* GetSlottedWeapon() const { return SlottedWeapon; }
	void SetSlottedWeapon(AWeapon* Weapon) { SlottedWeapon = Weapon; }

private:

	AWeapon* SlottedWeapon;
	
};
