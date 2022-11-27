// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AOSHUD.h"
#include "HUD/AOSCharacterOverlay.h"
#include "HUD/Inventory.h"
#include "HUD/InventorySlot.h"
#include "HUD/ItemInventorySlot.h"
#include "Weapons/Weapon.h"
#include "GameFramework/PlayerController.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Engine/Texture2D.h"

void AAOSHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AddOverlay();
}

void AAOSHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateInventorySlot();
	CreateItemInventorySlot();
}

void AAOSHUD::AddOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UAOSCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}

	if (CharacterOverlay && CharacterOverlay->InventoryWidget)
	{
		CharacterOverlay->InventoryWidget->BindButtonEvent();
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

void AAOSHUD::CreateInventorySlot()
{
	if (CharacterOverlay == nullptr || CharacterOverlay->InventoryWidget == nullptr) 
		return;
	
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && InventorySlotClass)
	{
		int32 SlotCount = CharacterOverlay->InventoryWidget->SlotArray.Num() + 1;
		int32 Row = SlotCount == 0 ? SlotCount : SlotCount / 5;

		for (int32 i = 0; i < 5; i++)
		{
			UInventorySlot* Slot = CreateWidget<UInventorySlot>(PlayerController, InventorySlotClass);

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

int32 AAOSHUD::GetInventorySlotCount()
{
	return CharacterOverlay->InventoryWidget->SlotArray.Num();
}

void AAOSHUD::UpdateInventory()
{
	if (CharacterOverlay == nullptr || CharacterOverlay->InventoryWidget == nullptr || CharacterOverlay->InventoryWidget->InventoryGridPanel == nullptr) 
		return;

	for (int32 i = 0; i < CharacterOverlay->InventoryWidget->SlotArray.Num(); i++)
	{
		UUniformGridSlot* GridSlot =
			CharacterOverlay->InventoryWidget->InventoryGridPanel->AddChildToUniformGrid(CharacterOverlay->InventoryWidget->SlotArray[i], i / 5, i % 5);
		GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
}

void AAOSHUD::AddWeaponToSlot(int32 SlotNum, AWeapon* Weapon)
{
	if (CharacterOverlay && CharacterOverlay->InventoryWidget)
	{
		CharacterOverlay->InventoryWidget->SlotArray[SlotNum]->SetSlottedWeapon(Weapon);
		Weapon->SetInventorySlot(CharacterOverlay->InventoryWidget->SlotArray[SlotNum]);

		if (CharacterOverlay->InventoryWidget->SlotArray[SlotNum]->InventorySlotIcon && Weapon->GetItemIcon())
		{
			FSlateBrush Brush;
			Brush.DrawAs = ESlateBrushDrawType::Image;
			CharacterOverlay->InventoryWidget->SlotArray[SlotNum]->InventorySlotIcon->SetBrush(Brush);
			CharacterOverlay->InventoryWidget->SlotArray[SlotNum]->InventorySlotIcon->SetBrushFromTexture(Weapon->GetItemIcon());
			CharacterOverlay->InventoryWidget->SlotArray[SlotNum]->BindSlotClickEvent();
		}
	}
}

void AAOSHUD::CreateItemInventorySlot()
{
	if (CharacterOverlay == nullptr || CharacterOverlay->InventoryWidget == nullptr) 
		return;

	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && ItemInventorySlotClass)
	{
		int32 SlotCount = CharacterOverlay->InventoryWidget->ItemSlotArray.Num() + 1;
		int32 Row = SlotCount == 0 ? SlotCount : SlotCount / 5;

		for (int32 i = 0; i < 5; i++)
		{
			UItemInventorySlot* ItemSlot = CreateWidget<UItemInventorySlot>(PlayerController, ItemInventorySlotClass);

			CharacterOverlay->InventoryWidget->ItemSlotArray.Add(ItemSlot);

			if (CharacterOverlay->InventoryWidget->ItemInventoryGridPanel)
			{
				UUniformGridSlot* GridSlot = CharacterOverlay->InventoryWidget->ItemInventoryGridPanel->AddChildToUniformGrid(CharacterOverlay->InventoryWidget->ItemSlotArray[i + Row * 5], Row, i);
				GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
				GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
			}
		}
	}
}

void AAOSHUD::UpdateItemInventory()
{
	if (CharacterOverlay == nullptr || CharacterOverlay->InventoryWidget == nullptr || CharacterOverlay->InventoryWidget->ItemInventoryGridPanel == nullptr) 
		return;

	for (int32 i = 0; i < CharacterOverlay->InventoryWidget->ItemSlotArray.Num(); i++)
	{
		UUniformGridSlot* GridSlot =
			CharacterOverlay->InventoryWidget->ItemInventoryGridPanel->AddChildToUniformGrid(CharacterOverlay->InventoryWidget->ItemSlotArray[i], i / 5, i % 5);
		GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
}

void AAOSHUD::AddItemToSlot(int32 SlotNum, AItem* Item)
{
	if (CharacterOverlay && CharacterOverlay->InventoryWidget)
	{
		CharacterOverlay->InventoryWidget->ItemSlotArray[SlotNum]->SetSlottedItem(Item);
		Item->SetItemInventorySlot(CharacterOverlay->InventoryWidget->ItemSlotArray[SlotNum]);

		if (CharacterOverlay->InventoryWidget->ItemSlotArray[SlotNum]->ItemInventorySlotIcon && Item->GetItemIcon())
		{
			FSlateBrush Brush;
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.SetImageSize(FVector2D(110.f));
			CharacterOverlay->InventoryWidget->ItemSlotArray[SlotNum]->ItemInventorySlotIcon->SetBrush(Brush);
			CharacterOverlay->InventoryWidget->ItemSlotArray[SlotNum]->ItemInventorySlotIcon->SetBrushFromTexture(Item->GetItemIcon());
			CharacterOverlay->InventoryWidget->ItemSlotArray[SlotNum]->BindSlotClickEvent();
		}
	}
}

int32 AAOSHUD::GetItemInventorySlotCount()
{
	return CharacterOverlay->InventoryWidget->ItemSlotArray.Num();
}
