// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AOSController.h"
#include "HUD/AOSCharacterOverlay.h"
#include "HUD/AOSHUD.h"
#include "HUD/Inventory.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

void AAOSController::BeginPlay()
{
	Super::BeginPlay();

	AOSHUD = Cast<AAOSHUD>(GetHUD());
}

void AAOSController::SetHUDHealthBar(float HealthAmount, float MaxHealthAmount)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->HealthBar)
	{
		const float HealthPercent = HealthAmount / MaxHealthAmount;
		AOSHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
	}
}

void AAOSController::SetHUDManaBar(float ManaAmount)
{

}

void AAOSController::SetHUDStaminaBar(float StaminaAmount, float MaxStaminaAmount)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->StaminaBar)
	{
		const float StaminaPercent = StaminaAmount / MaxStaminaAmount;
		AOSHUD->CharacterOverlay->StaminaBar->SetPercent(StaminaPercent);
	}
}

void AAOSController::SetHUDItemIcon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->ItemIcon)
	{
		AOSHUD->CharacterOverlay->ItemIcon->SetBrushFromTexture(Icon);
	}
}

void AAOSController::SetHUDEquippedWeaponIcon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->EquippedWeaponIcon)
	{
		AOSHUD->CharacterOverlay->EquippedWeaponIcon->SetBrushFromTexture(Icon);
	}
}

void AAOSController::SetHUDWeaponQuickSlot1Icon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->WeaponQuickSlot1Icon)
	{
		AOSHUD->CharacterOverlay->WeaponQuickSlot1Icon->SetBrushFromTexture(Icon);
	}
}

void AAOSController::SetHUDWeaponQuickSlot2Icon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->WeaponQuickSlot2Icon)
	{
		AOSHUD->CharacterOverlay->WeaponQuickSlot2Icon->SetBrushFromTexture(Icon);
	}
}

void AAOSController::HUDInventoryOn(bool IsInventoryOn)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->InventoryWidget && AOSHUD->CharacterOverlay->AnimInventoryFadeInOut)
	{
		if (IsInventoryOn)
		{
			AOSHUD->CharacterOverlay->InventoryWidget->SetVisibility(ESlateVisibility::Visible);
			AOSHUD->CharacterOverlay->InventoryWidget->SetIsEnabled(true);
			AOSHUD->CharacterOverlay->PlayAnimation(AOSHUD->CharacterOverlay->AnimInventoryFadeInOut);
		}
		else
		{
			GetWorldTimerManager().SetTimer(InventoryOffDelayTimer, this, &AAOSController::HUDInventoryOff, InventoryOffDelayTime);
			AOSHUD->CharacterOverlay->PlayAnimation(AOSHUD->CharacterOverlay->AnimInventoryFadeInOut, 0.f, 1, EUMGSequencePlayMode::Reverse);
		}
	}
}

void AAOSController::HUDInventoryOff()
{
	AOSHUD->CharacterOverlay->InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	AOSHUD->CharacterOverlay->InventoryWidget->SetIsEnabled(false);
}

void AAOSController::SetHUDLoadedAmmoText(int32 Ammo)
{
	AOSHUD = AOSHUD == nullptr ? Cast<AAOSHUD>(GetHUD()) : AOSHUD;

	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->LoadedAmmoText)
	{
		AOSHUD->CharacterOverlay->LoadedAmmoText->SetText(FText::FromString(FString::FromInt(Ammo)));
	}
}

void AAOSController::SetHUDTotalAmmoText(int32 Ammo)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->TotalAmmoText)
	{
		AOSHUD->CharacterOverlay->TotalAmmoText->SetText(FText::FromString(FString::FromInt(Ammo)));
	}
}

void AAOSController::HUDAmmoInfoOn()
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->AmmoInfoBoxOverlay && AOSHUD->CharacterOverlay->AnimAmmoInfoAppear)
	{
		AOSHUD->CharacterOverlay->AmmoInfoBoxOverlay->SetVisibility(ESlateVisibility::Visible);
		AOSHUD->CharacterOverlay->PlayAnimation(AOSHUD->CharacterOverlay->AnimAmmoInfoAppear);
	}
}


