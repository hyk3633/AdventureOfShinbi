

#include "Player/AOSController.h"
#include "Player/AOSCharacter.h"
#include "System/AOSGameModeBase.h"
#include "System/AOSGameInstance.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
#include "Items/ItemRecovery.h"
#include "Items/ItemAmmo.h"
#include "HUD/AOSCharacterOverlay.h"
#include "HUD/AOSHUD.h"
#include "HUD/Inventory.h"
#include "HUD/ItemInventorySlot.h"
#include "Components/CombatComponent.h"
#include "Components/ItemComponent.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void AAOSController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AAOSController::BeginPlay()
{
	Super::BeginPlay();

}

void AAOSController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	AOSHUD = GetHUD<AAOSHUD>();
	if (AOSHUD)
	{
		AOSHUD->CharacterOverlay->InventoryWidget->OnItemQuickSlotSelected.BindUObject(this, &AAOSController::EquipToItemQuickSlot);
	}

	GameMode = GetWorld()->GetAuthGameMode<AAOSGameModeBase>();
	if (GameMode)
	{
		if (GetWorld()->GetGameInstance<UAOSGameInstance>()->GetShouldLoadData())
		{
			GetWorld()->GetGameInstance<UAOSGameInstance>()->DeacitavateShouldLoadData();
			GetWorld()->GetAuthGameMode<AAOSGameModeBase>()->LoadPlayerData();
		}

		if (GameMode->IsPlayerRespawn())
		{
			AAOSCharacter* Cha = Cast<AAOSCharacter>(aPawn);
			if (Cha)
			{
				Cha->RestartPlayerCharacter();
			}
		}
	}
}

void AAOSController::SetHUDHealthBar(float HealthAmount, float MaxHealthAmount)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->HealthBar)
	{
		const float HealthPercent = HealthAmount / MaxHealthAmount;
		AOSHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
	}
}

void AAOSController::SetHUDManaBar(float ManaAmount, float MaxManaAmount)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->ManaBar)
	{
		const float ManaPercent = ManaAmount / MaxManaAmount;
		AOSHUD->CharacterOverlay->ManaBar->SetPercent(ManaPercent);
	}
}

void AAOSController::SetHUDStaminaBar(float StaminaAmount, float MaxStaminaAmount)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->StaminaBar)
	{
		const float StaminaPercent = StaminaAmount / MaxStaminaAmount;
		AOSHUD->CharacterOverlay->StaminaBar->SetPercent(StaminaPercent);
	}
}

void AAOSController::BossHealthBarOn()
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->BossHealthBar)
	{
		AOSHUD->CharacterOverlay->BossHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AAOSController::BossHealthBarOff()
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->BossHealthBar)
	{
		AOSHUD->CharacterOverlay->BossHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AAOSController::SetHUDBossHealthBar(float HealthPercentage)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->BossHealthBar)
	{
		AOSHUD->CharacterOverlay->BossHealthBar->SetPercent(HealthPercentage);
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

void AAOSController::EquipToItemQuickSlot(int8 SlotIndex, UImage* QuickSlotIcon, UTextBlock* QuickSlotCountText)
{
	const int32 ItemIndex = GameMode->GetItemIndex(SelectedItem);

	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemIndex]->ButtonCancel->SetVisibility(ESlateVisibility::Hidden);
	AOSHUD->CharacterOverlay->InventoryWidget->AllQuickSlotButtonDisabled();
	GetWorldTimerManager().ClearTimer(QuickSlotAnimationTimer);

	if (SelectedItem)
	{
		if (QuickSlotIcon && QuickSlotCountText)
		{
			// 선택한 아이템이 퀵슬롯에 이미 등록되어 있을 경우
			AItem* QuickSlotItem = GameMode->GetQuickSlotItem(SlotIndex);
			if (QuickSlotItem == SelectedItem)
			{
				AItemRecovery* Recovery = Cast<AItemRecovery>(QuickSlotItem);
				Recovery->SetQuickSlotIndex(-1);
				SelectedItem = nullptr;
				return;
			}
			// 퀵슬롯이 이미 차 있을 경우
			else if (QuickSlotItem != nullptr)
			{
				AItemRecovery* Recovery = Cast<AItemRecovery>(QuickSlotItem);
				Recovery->SetQuickSlotIndex(-1);
				const int32 QuickSlotItemIndex = GameMode->GetItemIndex(QuickSlotItem);
				AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[QuickSlotItemIndex]->ItemQuickSlotButtonText->SetText(FText::FromString(TEXT("퀵슬롯에 장착")));
				AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[QuickSlotItemIndex]->SetItemEquippedQuickSlot(false);
			}
			else
			{
				AItemRecovery* Recovery = Cast<AItemRecovery>(SelectedItem);
				Recovery->SetQuickSlotIndex(SlotIndex);
			}

			if (CheckQuickSlotArrayIsEmpty())
			{
				SetEquippedQuickItem(SlotIndex, SelectedItem);
			}

			GameMode->SetQuickSlotItem(SlotIndex, SelectedItem, QuickSlotIcon, QuickSlotCountText);

			FSlateBrush Brush;
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.SetImageSize(FVector2D(110.f));

			QuickSlotIcon->SetBrush(Brush);
			QuickSlotIcon->SetBrushFromTexture(SelectedItem->GetItemIcon());
			QuickSlotCountText->SetText(FText::FromString(FString::FromInt(GetItemCount(SelectedItem))));
			QuickSlotCountText->SetVisibility(ESlateVisibility::Visible);

			AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemIndex]->ButtonEquipToQuickSlot->SetIsEnabled(true);
			AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemIndex]->ButtonCancel->SetVisibility(ESlateVisibility::Hidden);
			AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemIndex]->DeactivateItemInventorySlotClick();
			AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemIndex]->SetItemEquippedQuickSlot(true);
			AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemIndex]->ItemQuickSlotButtonText->SetText(FText::FromString(TEXT("퀵슬롯 장착 해제")));
			SelectedItem = nullptr;
		}
	}
}

void AAOSController::SetEquippedQuickItem(int8 SlotIndex, AItem* Item)
{
	GameMode->SetActivatedQuickSlotNumber(SlotIndex);
	AItemRecovery* Recovery = Cast<AItemRecovery>(Item);
	Recovery->SetQuickSlotIndex(SlotIndex);

	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.SetImageSize(FVector2D(110.f));

	AOSHUD->CharacterOverlay->EquippedItemIcon->SetBrush(Brush);
	AOSHUD->CharacterOverlay->EquippedItemIcon->SetBrushFromTexture(Item->GetItemIcon());
	AOSHUD->CharacterOverlay->EquippedItemSlotCountText->SetText(FText::FromString(FString::FromInt(GetItemCount(Item))));
	AOSHUD->CharacterOverlay->EquippedItemSlotCountText->SetVisibility(ESlateVisibility::Visible);
}

int32 AAOSController::GetItemCount(AItem* Item)
{
	if (Item->GetItemType() == EItemType::EIT_Recovery)
	{
		AItemRecovery* IR = Cast<AItemRecovery>(Item);
		const int32 RecoveryItemCount = GameMode->GetRecoveryItemCount(IR->GetRecoveryType());
		return RecoveryItemCount;
	}
	else if (Item->GetItemType() == EItemType::EIT_Ammo)
	{
		AItemAmmo* IA = Cast<AItemAmmo>(Item);
		const int32 AmmoQuantity = GameMode->GetAmmoQuantity(IA->GetAmmoType());
		return AmmoQuantity;
	}
	else
	{
		return 0;
	}
}

bool AAOSController::CheckQuickSlotArrayIsEmpty()
{
	for (int8 i = 0; i < 5; i++)
	{
		if (GameMode->GetQuickSlotItem(i) != nullptr)
		{
			return false;
		}
	}

	return true;
}

void AAOSController::SetSelectedItem(AItem* Item)
{
	SelectedItem = Item;
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

void AAOSController::SetHUDCrosshairs(FCrosshairs Ch)
{
	AOSHUD->CrosshairCenter = Ch.CrosshairCenter;
	AOSHUD->CrosshairLeft	= Ch.CrosshairLeft;
	AOSHUD->CrosshairRight	= Ch.CrosshairRight;
	AOSHUD->CrosshairTop	= Ch.CrosshairTop;
	AOSHUD->CrosshairBottom = Ch.CrosshairBottom;
}

void AAOSController::EraseHUDCrosshairs()
{
	AOSHUD->CrosshairCenter = nullptr;
	AOSHUD->CrosshairLeft	= nullptr;
	AOSHUD->CrosshairRight	= nullptr;
	AOSHUD->CrosshairTop	= nullptr;
	AOSHUD->CrosshairBottom = nullptr;
}

void AAOSController::SetCrosshairSpread(float Spread)
{
	AOSHUD->CrosshairSpread = Spread;
}

int32 AAOSController::GetHUDInventorySlotCount()
{
	return AOSHUD->CharacterOverlay->InventoryWidget->SlotArray.Num();
}

void AAOSController::CreateHUDInventorySlot()
{
	AOSHUD->CreateInventorySlot();
}

void AAOSController::AddWeaponToSlot(int32 SlotNum, AWeapon* Weapon)
{
	AOSHUD->AddWeaponToSlot(SlotNum, Weapon);
}

void AAOSController::UpdateInventory(UInventorySlot* Slot)
{
	AOSHUD->CharacterOverlay->InventoryWidget->SlotArray.Remove(Slot);
	AOSHUD->CharacterOverlay->InventoryWidget->SlotArray.Add(Slot);
	AOSHUD->UpdateInventory();
}

void AAOSController::UpdateItemInventory(int32 Index)
{
	UItemInventorySlot* Slot = AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[Index];
	Slot->InitializeIcon();
	Slot->SetSlottedItem(nullptr);

	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray.Remove(Slot);
	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray.Add(Slot);
	AOSHUD->UpdateItemInventory();
}

void AAOSController::CreateHUDItemInventorySlot()
{
	if (GameMode->GetItemCount() == AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray.Num())
	{
		AOSHUD->CreateItemInventorySlot();
	}
}

void AAOSController::ItemChange()
{
	GameMode->SetActivatedQuickSlotNumber((GameMode->GetActivatedQuickSlotNumber() + 1) % 5);

	for (int8 i = 0; i < 4; i++)
	{
		AItem* QuickSlotItem = GameMode->GetQuickSlotItem(GameMode->GetActivatedQuickSlotNumber());
		if (QuickSlotItem)
		{
			FSlateBrush Brush;
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.SetImageSize(FVector2D(110.f));
			AOSHUD->CharacterOverlay->EquippedItemIcon->SetBrush(Brush);
			AOSHUD->CharacterOverlay->EquippedItemIcon->SetBrushFromTexture(QuickSlotItem->GetItemIcon());
			AOSHUD->CharacterOverlay->EquippedItemSlotCountText->SetText(FText::FromString(FString::FromInt(GetItemCount(QuickSlotItem))));
			AOSHUD->CharacterOverlay->EquippedItemSlotCountText->SetVisibility(ESlateVisibility::Visible);
			return;
		}
		else
		{
			GameMode->SetActivatedQuickSlotNumber((GameMode->GetActivatedQuickSlotNumber() + 1) % 5);
		}
	}
}

void AAOSController::UpdateAmmo(EAmmoType AmmoType)
{
	const int32 AmmoQuatity = GameMode->GetAmmoQuantity(AmmoType);
	FText AmmoCount = FText::FromString(FString::FromInt(AmmoQuatity));

	const int32 AmmoIndex = GameMode->GetAmmoIndex(AmmoType);
	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[AmmoIndex]->ItemSlotCountText->SetText(AmmoCount);
}

void AAOSController::AddItemToSlot(AItem* Item)
{
	AOSHUD->AddItemToSlot(GameMode->GetItemCount() - 1, Item);
}

void AAOSController::BindToItemSlot(int32 Index)
{
	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[Index]->OnItemMenuSelect.BindUObject(GetPawn<AAOSCharacter>()->GetItemComp(), &UItemComponent::ItemUseOrEquip);
}

void AAOSController::SetItemSlotCountText(int32 Index, int32 Count)
{
	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[Index]->SetItemSlotCountText(Count);
}

void AAOSController::DisableItemSlotButton()
{
	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[GameMode->GetItemCount() - 1]->ItemInventorySlotIconButton->SetIsEnabled(false);
}

void AAOSController::AllHUDQuickSlotButtonEnabled()
{
	AOSHUD->CharacterOverlay->InventoryWidget->AllQuickSlotButtonEnabled();
	GetWorldTimerManager().SetTimer(QuickSlotAnimationTimer, this, &AAOSController::PlayQuickSlotActivateAnimation, QuickSlotAnimationTime, true, 0.f);
}

void AAOSController::AllHUDQuickSlotButtonDisabled()
{
	AOSHUD->CharacterOverlay->InventoryWidget->AllQuickSlotButtonDisabled();
	GetWorldTimerManager().ClearTimer(QuickSlotAnimationTimer);
}

void AAOSController::SetHUDItemSlotDismount(int32 Index)
{
	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[Index]->
		ItemQuickSlotButtonText->SetText(FText::FromString(TEXT("퀵슬롯에 장착")));
	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[Index]->SetItemEquippedQuickSlot(false);
}

void AAOSController::DeactivateItemInventorySlotClick(int32 Index)
{
	AOSHUD->CharacterOverlay->InventoryWidget->ItemSlotArray[Index]->DeactivateItemInventorySlotClick();
}

void AAOSController::ClearEquippedItemSlotHUD()
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
	Brush.SetImageSize(FVector2D(110.f));
	AOSHUD->CharacterOverlay->EquippedItemIcon->SetBrush(Brush);
	AOSHUD->CharacterOverlay->EquippedItemSlotCountText->SetVisibility(ESlateVisibility::Hidden);
}

void AAOSController::PlayQuickSlotActivateAnimation()
{
	AOSHUD->CharacterOverlay->InventoryWidget->PlayAnimation
	(
		AOSHUD->CharacterOverlay->InventoryWidget->Highlighting,
		0.f,
		1,
		EUMGSequencePlayMode::PingPong,
		1.f,
		true
	);
}

void AAOSController::SetEquippedItemSlotCountText(FText Text)
{
	AOSHUD->CharacterOverlay->EquippedItemSlotCountText->SetText(Text);
}

void AAOSController::SetQuickSlotItemAuto()
{
	if (CheckQuickSlotArrayIsEmpty() == false)
	{
		for (int8 i = 0; i < GameMode->GetQuickSlotItemArrLength(); i++)
		{
			FQuickSlotItem QItem = GameMode->GetQuickSlotItemArr(i);
			if (QItem.QuickSlotItem)
			{
				SelectedItem = QItem.QuickSlotItem;
				EquipToItemQuickSlot(i, QItem.QuickSlotIcon, QItem.QuickSlotItemCountText);
			}
		}
	}
}

void AAOSController::EnrollToItemQuickSlot(int8 Index)
{
	AOSHUD = GetHUD<AAOSHUD>();
	if (AOSHUD)
	{
		switch (Index)
		{
		case 0:
			AOSHUD->CharacterOverlay->InventoryWidget->QuickSlot1Clicked();
			break;
		case 1:
			AOSHUD->CharacterOverlay->InventoryWidget->QuickSlot2Clicked();
			break;
		case 2:
			AOSHUD->CharacterOverlay->InventoryWidget->QuickSlot3Clicked();
			break;
		case 3:
			AOSHUD->CharacterOverlay->InventoryWidget->QuickSlot4Clicked();
			break;
		case 4:
			AOSHUD->CharacterOverlay->InventoryWidget->QuickSlot5Clicked();
			break;
		}
	}
}

void AAOSController::HideHUD()
{
	if (AOSHUD && AOSHUD->CharacterOverlay)
	{
		AOSHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AAOSController::ShowHUD()
{
	if (AOSHUD && AOSHUD->CharacterOverlay)
	{
		AOSHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}
