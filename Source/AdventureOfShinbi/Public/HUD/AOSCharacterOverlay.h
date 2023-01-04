

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AOSCharacterOverlay.generated.h"

/**
 * 메인 UI 위젯을 모두 표시하는 오버레이 클래스
 */

class UProgressBar;
class UTextBlock;
class UOverlay;
class UImage;
class UInventory;
class UWidgetAnimation;

UCLASS()
class ADVENTUREOFSHINBI_API UAOSCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	/** 상단 플레이어 스탯 UI */

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	/** 보스 체력바 UI */

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossHealthBar;

	/** 탄약 정보 UI */

	UPROPERTY(meta = (BindWidget))
	UOverlay* AmmoInfoBoxOverlay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadedAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalAmmoText;

	/** 하단 아이템 정보 UI */

	UPROPERTY(meta = (BindWidget))
	UImage* EquippedItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EquippedItemSlotCountText;

	UPROPERTY(meta = (BindWidget))
	UImage* EquippedWeaponIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponQuickSlot1Icon;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponQuickSlot2Icon;

	/** 인벤토리 위젯 UI */

	UPROPERTY(meta = (BindWidget))
	UInventory* InventoryWidget;
	
	/** 위젯 애니메이션 UI */

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* AnimInventoryFadeInOut;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* AnimAmmoInfoAppear;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* AnimAmmoInfoFadeOut;

};
