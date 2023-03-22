
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

	/** 스탯 ui 세팅 */

	void SetHUDHealthBar(float HealthAmount, float MaxHealthAmount);
	void SetHUDManaBar(float ManaAmount, float MaxManaAmount);
	void SetHUDStaminaBar(float StaminaAmount, float MaxStaminaAmount);

	/** 보스 ui 세팅 */

	void BossHealthBarOn();
	void BossHealthBarOff();
	void SetHUDBossHealthBar(float HealthPercentage);

	/** 메인 ui 세팅 */

	void SetHUDEquippedItemIcon(UTexture2D* Icon);
	void SetHUDEquippedWeaponIcon(UTexture2D* Icon);
	void SetHUDWeaponQuickSlot1Icon(UTexture2D*Icon);
	void SetHUDWeaponQuickSlot2Icon(UTexture2D* Icon);

	void HUDInventoryOn(bool IsInventoryOn);

	/** 탄약 ui 세팅 */

	void SetHUDLoadedAmmoText(int32 Ammo);
	void SetHUDTotalAmmoText(int32 Ammo);
	void HUDAmmoInfoOn();
	void HUDAmmoInfoOff();

	/** 인벤토리 ui 세팅 */

	void SetHUDInventoryEquippedWeaponSlotIcon(UTexture2D* Icon);
	void SetHUDInventoryQuickSlot1Icon(UTexture2D* Icon);
	void SetHUDInventoryQuickSlot2Icon(UTexture2D* Icon);

	/** 크로스헤어 세팅 */

	void SetHUDCrosshairs(FCrosshairs Ch);

	void EraseHUDCrosshairs();

	void SetCrosshairSpread(float Spread);

	/** 인벤토리 */

	int32 GetHUDInventorySlotCount();

	void CreateHUDInventorySlot();

	void AddWeaponToSlot(int32 SlotNum, AWeapon* Weapon);

	void UpdateInventory(UInventorySlot* Slot);

	/** 아이템 인벤토리 */

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

	void UpdateQuickSlotItemText(int8 Index, int32 ItemCount);

	/** HUD 숨김 및 표시 */

	void HideHUD();

	void ShowHUD();

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	void HUDInventoryOff();

	/** 
	* 아이템 퀵슬롯에 아이템을 등록하는 함수
	* @param SlotIndex 퀵슬롯의 인덱스
	* @param QuickSlotIcon 퀵슬롯의 아이콘 이미지
	* @param QuickSlotCountText 퀵슬롯의 텍스트블록
	*/
	UFUNCTION()
	void EquipToItemQuickSlot(int8 SlotIndex, UImage* QuickSlotIcon, UTextBlock* QuickSlotCountText);

	/** 
	* 아이템 개수 반환
	* @param Item 개수를 반환할 아이템
	*/
	int32 GetItemCount(AItem* Item);

	/** 퀵슬롯에 등록된 아이템이 없으면 true 를 반환 */
	bool CheckQuickSlotArrayIsEmpty();

private:

	UPROPERTY()
	AAOSGameModeBase* GameMode;

	UPROPERTY()
	AAOSHUD* AOSHUD;

	/** 퀵슬롯에 장착할 아이템 */
	UPROPERTY()
	AItem* SelectedItem;

	FTimerHandle InventoryOffDelayTimer;

	float InventoryOffDelayTime = 1.f;

	FTimerHandle QuickSlotAnimationTimer;

	float QuickSlotAnimationTime = 1.0f;

	int8 ActivatedQuickSlotNumber = 0;

public:

	/** 퀵슬롯에 장착할 아이템 설정 */
	void SetSelectedItem(AItem* Item);

};
