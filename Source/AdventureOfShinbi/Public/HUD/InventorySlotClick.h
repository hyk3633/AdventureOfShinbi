// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotClick.generated.h"

/**
 * 
 */

class UButton;

UCLASS()
class ADVENTUREOFSHINBI_API UInventorySlotClick : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonEquip;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonEquipToQuickSlot1;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonEquipToQuickSlot2;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonDiscardWeapon;

};
