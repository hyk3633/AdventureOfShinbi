// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AOSController.h"
#include "HUD/AOSCharacterOverlay.h"
#include "HUD/AOSHUD.h"
#include "HUD/Inventory.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"

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

void AAOSController::SetHUDEquippedItemIcon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->EquippedItemIcon)
	{
		AOSHUD->CharacterOverlay->EquippedItemIcon->SetBrushFromTexture(Icon);
	}
}

void AAOSController::SetHUDEquippedWeaponIcon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->EquippedWeaponIcon)
	{
		FSlateBrush Brush;
		if (Icon == nullptr)
		{
			Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
			AOSHUD->CharacterOverlay->EquippedWeaponIcon->SetBrush(Brush);
		}
		else
		{
			Brush.DrawAs = ESlateBrushDrawType::Image;
			AOSHUD->CharacterOverlay->EquippedWeaponIcon->SetBrush(Brush);
			AOSHUD->CharacterOverlay->EquippedWeaponIcon->SetBrushFromTexture(Icon);
		}
	}
}

void AAOSController::SetHUDWeaponQuickSlot1Icon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->WeaponQuickSlot1Icon)
	{
		FSlateBrush Brush;
		if (Icon == nullptr)
		{
			Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
			AOSHUD->CharacterOverlay->WeaponQuickSlot1Icon->SetBrush(Brush);
		}
		else
		{
			Brush.DrawAs = ESlateBrushDrawType::Image;
			AOSHUD->CharacterOverlay->WeaponQuickSlot1Icon->SetBrush(Brush);
			AOSHUD->CharacterOverlay->WeaponQuickSlot1Icon->SetBrushFromTexture(Icon);
		}
	}
}

void AAOSController::SetHUDWeaponQuickSlot2Icon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->WeaponQuickSlot2Icon)
	{
		FSlateBrush Brush;
		if (Icon == nullptr)
		{
			Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
			AOSHUD->CharacterOverlay->WeaponQuickSlot2Icon->SetBrush(Brush);
		}
		else
		{
			Brush.DrawAs = ESlateBrushDrawType::Image;
			AOSHUD->CharacterOverlay->WeaponQuickSlot2Icon->SetBrush(Brush);
			AOSHUD->CharacterOverlay->WeaponQuickSlot2Icon->SetBrushFromTexture(Icon);
		}
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

			AOSHUD->CharacterOverlay->InventoryWidget->bIsFocusable = true;

			FInputModeGameAndUI InputModeGameAndUI;
			SetInputMode(InputModeGameAndUI);
			SetShowMouseCursor(true);
		}
		else
		{
			GetWorldTimerManager().SetTimer(InventoryOffDelayTimer, this, &AAOSController::HUDInventoryOff, InventoryOffDelayTime);
			AOSHUD->CharacterOverlay->PlayAnimation(AOSHUD->CharacterOverlay->AnimInventoryFadeInOut, 0.f, 1, EUMGSequencePlayMode::Reverse);

			FInputModeGameOnly InputModeGameOnly;
			SetInputMode(InputModeGameOnly);
			SetShowMouseCursor(false);
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

void AAOSController::HUDAmmoInfoOff()
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->AmmoInfoBoxOverlay && AOSHUD->CharacterOverlay->AnimAmmoInfoAppear)
	{
		AOSHUD->CharacterOverlay->AmmoInfoBoxOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AAOSController::SetHUDInventoryEquippedWeaponSlotIcon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->InventoryWidget && AOSHUD->CharacterOverlay->InventoryWidget->InventoryEquippedWeaponSlotIcon)
	{
		FSlateBrush Brush;
		if (Icon == nullptr)
		{
			Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
			Brush.SetImageSize(FVector2D(150.f));
			AOSHUD->CharacterOverlay->InventoryWidget->InventoryEquippedWeaponSlotIcon->SetBrush(Brush);
		}
		else
		{
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.SetImageSize(FVector2D(150.f));
			AOSHUD->CharacterOverlay->InventoryWidget->InventoryEquippedWeaponSlotIcon->SetBrush(Brush);
			AOSHUD->CharacterOverlay->InventoryWidget->InventoryEquippedWeaponSlotIcon->SetBrushFromTexture(Icon);
		}
	}
}

void AAOSController::SetHUDInventoryQuickSlot1Icon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->InventoryWidget && AOSHUD->CharacterOverlay->InventoryWidget->InventoryQuickSlot1Icon)
	{
		FSlateBrush Brush;
		if (Icon == nullptr)
		{
			Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
			Brush.SetImageSize(FVector2D(120.f));
			AOSHUD->CharacterOverlay->InventoryWidget->InventoryQuickSlot1Icon->SetBrush(Brush);
		}
		else
		{
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.SetImageSize(FVector2D(120.f));
			AOSHUD->CharacterOverlay->InventoryWidget->InventoryQuickSlot1Icon->SetBrush(Brush);
			AOSHUD->CharacterOverlay->InventoryWidget->InventoryQuickSlot1Icon->SetBrushFromTexture(Icon);
		}
	}
}

void AAOSController::SetHUDInventoryQuickSlot2Icon(UTexture2D* Icon)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->InventoryWidget && AOSHUD->CharacterOverlay->InventoryWidget->InventoryQuickSlot2Icon)
	{
		FSlateBrush Brush;
		if (Icon == nullptr)
		{
			Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
			Brush.SetImageSize(FVector2D(120.f));
			AOSHUD->CharacterOverlay->InventoryWidget->InventoryQuickSlot2Icon->SetBrush(Brush);
		}
		else
		{
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.SetImageSize(FVector2D(120.f));
			AOSHUD->CharacterOverlay->InventoryWidget->InventoryQuickSlot2Icon->SetBrush(Brush);
			AOSHUD->CharacterOverlay->InventoryWidget->InventoryQuickSlot2Icon->SetBrushFromTexture(Icon);
		}
	}
}

void AAOSController::SetHUDItemInventoryQuickSlotIcon(UTexture2D* Icon)
{

}
