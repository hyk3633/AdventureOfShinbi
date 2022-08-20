

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AOSCharacterOverlay.generated.h"

/**
 * 
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

	// ��� �÷��̾� ����

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	// ź�� ����

	UPROPERTY(meta = (BindWidget))
	UOverlay* AmmoInfoBoxOverlay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadedAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalAmmoText;

	// �ϴ� ������ ����

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* EquippedWeaponIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponQuickSlot1Icon;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponQuickSlot2Icon;

	// �κ��丮 ����

	UPROPERTY(meta = (BindWidget))
	UInventory* InventoryWidget;
	
	// ���� �ִϸ��̼�

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* AnimInventoryFadeInOut;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* AnimAmmoInfoAppear;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* AnimAmmoInfoFadeOut;

};
