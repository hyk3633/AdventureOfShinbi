
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Types/AmmoType.h"
#include "Types/ItemType.h"
#include "AOSController.generated.h"

/**
 * 
 */

class AAOSHUD;
class AAOSGameModeBase;
class UTexture2D;
class AWeapon;
class AItem;
class UImage;
class UButton;
class UTextBlock;
class UInventorySlot;
struct FCrosshairs;

UCLASS()
class ADVENTUREOFSHINBI_API AAOSController : public APlayerController
{
	GENERATED_BODY()
	
public:

	// 스탯 ui 세팅

	void SetHUDHealthBar(float HealthAmount, float MaxHealthAmount);
	void SetHUDManaBar(float ManaAmount, float MaxManaAmount);
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

	//

	void SetHUDCrosshairs(FCrosshairs Ch);

	void EraseHUDCrosshairs();

	void SetCrosshairSpread(float Spread);

	int32 GetHUDInventorySlotCount();

	void CreateHUDInventorySlot();

	void AddWeaponToSlot(int32 SlotNum, AWeapon* Weapon);

	void UpdateInventory(UInventorySlot* Slot);

	//

	void UpdateItemInventory(int32 Index);

	void CreateHUDItemInventorySlot();

	void ItemChange();

	void UpdateAmmo(EAmmoType AmmoType);

	void AddItemToSlot(AItem* Item);

	void BindToItemSlot(int32 Index);

	void SetItemSlotCountText(int32 Index, int32 Count);

	void DisableItemSlotButton();

	void AllHUDQuickSlotButtonEnabled();

	void AllHUDQuickSlotButtonDisabled();

	void SetHUDItemSlotDismount(int32 Index);

	void DeactivateItemInventorySlotClick(int32 Index);

	void ClearEquippedItemSlotHUD();

	void PlayQuickSlotActivateAnimation();

	void SetEquippedItemSlotCountText(FText Text);

	void SetQuickSlotItemAuto();

	void SetEquippedQuickItem(int8 SlotIndex, AItem* Item);

	void EnrollToItemQuickSlot(int8 Index);

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	void HUDInventoryOff();

	UFUNCTION()
	void EquipToItemQuickSlot(int8 SlotIndex, UImage* QuickSlotIcon, UTextBlock* QuickSlotCountText);

	int32 GetItemCount(AItem* Item);

	bool CheckQuickSlotArrayIsEmpty();

private:

	AAOSGameModeBase* GameMode;

	AAOSHUD* AOSHUD;

	AItem* SelectedItem;

	FTimerHandle InventoryOffDelayTimer;

	float InventoryOffDelayTime = 1.f;

	FTimerHandle QuickSlotAnimationTimer;

	float QuickSlotAnimationTime = 1.0f;

	int8 ActivatedQuickSlotNumber = 0;

public:

	void SetSelectedItem(AItem* Item);

};
