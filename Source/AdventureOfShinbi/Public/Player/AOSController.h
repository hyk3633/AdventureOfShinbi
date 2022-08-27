// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AOSController.generated.h"

/**
 * 
 */

class AAOSHUD;
class UTexture2D;

UCLASS()
class ADVENTUREOFSHINBI_API AAOSController : public APlayerController
{
	GENERATED_BODY()
	
public:

	// 스탯 ui 세팅

	void SetHUDHealthBar(float HealthAmount, float MaxHealthAmount);
	void SetHUDManaBar(float ManaAmount);
	void SetHUDStaminaBar(float StaminaAmount, float MaxStaminaAmount);

	// 메인 ui 세팅

	void SetHUDEquippedItemIcon(UTexture2D* Icon);
	void SetHUDEquippedWeaponIcon(UTexture2D* Icon);
	void SetHUDWeaponQuickSlot1Icon(UTexture2D*Icon);
	void SetHUDWeaponQuickSlot2Icon(UTexture2D* Icon);

	void HUDInventoryOn(bool IsInventoryOn);

	// 탄약 ui 세팅

	void SetHUDLoadedAmmoText(int32 Ammo);
	void SetHUDTotalAmmoText(int32 Ammo);
	void HUDAmmoInfoOn();
	void HUDAmmoInfoOff();

	// 인벤토리 ui 세팅

	void SetHUDInventoryEquippedWeaponSlotIcon(UTexture2D* Icon);
	void SetHUDInventoryQuickSlot1Icon(UTexture2D* Icon);
	void SetHUDInventoryQuickSlot2Icon(UTexture2D* Icon);

	// 아이템 인벤토리 ui 세팅

	void SetHUDItemInventoryQuickSlotIcon(UTexture2D* Icon);

protected:

	virtual void BeginPlay() override;

	void HUDInventoryOff();

private:

	AAOSHUD* AOSHUD;

	FTimerHandle InventoryOffDelayTimer;

	float InventoryOffDelayTime = 1.f;
};
