// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AOSHUD.h"
#include "HUD/AOSCharacterOverlay.h"
#include "HUD/Inventory.h"
#include "HUD/InventorySlot.h"
#include "Weapons/Weapon.h"
#include "GameFramework/PlayerController.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Engine/Texture2D.h"

void AAOSHUD::BeginPlay()
{
	Super::BeginPlay();

	AddOverlay();

	CreateInventorySlot();
}

void AAOSHUD::AddOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UAOSCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AAOSHUD::DrawHUD()
{
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * CrosshairSpread;

		if (CrosshairCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(CrosshairCenter, ViewportCenter, Spread);
		}
		if (CrosshairLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(CrosshairLeft, ViewportCenter, Spread);
		}
		if (CrosshairRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(CrosshairRight, ViewportCenter, Spread);
		}
		if (CrosshairTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(CrosshairTop, ViewportCenter, Spread);
		}
		if (CrosshairBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(CrosshairBottom, ViewportCenter, Spread);
		}
	}
}

void AAOSHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
	);
}

void AAOSHUD::SetCrosshairSpread(float Spread)
{
	CrosshairSpread = Spread;
}

void AAOSHUD::CreateInventorySlot()
{
	if (CharacterOverlay == nullptr || CharacterOverlay->InventoryWidget == nullptr) return;
	
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && InventorySlotClass)
	{
		int8 SlotCount = CharacterOverlay->InventoryWidget->SlotArray.Num() + 1;
		int8 Row = SlotCount == 0 ? SlotCount : SlotCount / 5;

		for (int8 i = 0; i < 5; i++)
		{
			UInventorySlot* Slot = CreateWidget<UInventorySlot>(PlayerController, InventorySlotClass);
			//Slot->RenderTransform.Scale.X = 1.5;
			//Slot->RenderTransform.Scale.Y = 1.5;
			CharacterOverlay->InventoryWidget->SlotArray.Add(Slot);

			if (CharacterOverlay->InventoryWidget->InventoryGridPanel)
			{
				UUniformGridSlot* GridSlot = CharacterOverlay->InventoryWidget->InventoryGridPanel->AddChildToUniformGrid(CharacterOverlay->InventoryWidget->SlotArray[i+Row*5], Row, i);
				GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
				GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
			}
		}
	}
}

void AAOSHUD::AddWeaponToSlot(int32 SlotNum, AWeapon* Weapon)
{
	if (CharacterOverlay && CharacterOverlay->InventoryWidget)
	{
		CharacterOverlay->InventoryWidget->SlotArray[SlotNum]->SetSlottedWeapon(Weapon);
		if (CharacterOverlay->InventoryWidget->SlotArray[SlotNum]->InventorySlotIcon && Weapon->GetWeaponIcon())
		{
			CharacterOverlay->InventoryWidget->SlotArray[SlotNum]->InventorySlotIcon->SetBrushFromTexture(Weapon->GetWeaponIcon());
			CharacterOverlay->InventoryWidget->SlotArray[SlotNum]->BindSlotClickEvent();
		}
	}
}