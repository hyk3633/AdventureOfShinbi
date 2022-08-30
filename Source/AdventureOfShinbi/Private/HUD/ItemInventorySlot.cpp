

#include "HUD/ItemInventorySlot.h"
#include "Items/Item.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UItemInventorySlot::BindSlotClickEvent()
{
	if (ItemInventorySlotIconButton)
	{
		ItemInventorySlotIconButton->OnClicked.AddDynamic(this, &UItemInventorySlot::ActivateItemInventorySlotClick);
	}
	if (ItemInventorySlotClick && SlottedItem)
	{
		if (ButtonUse)
		{
			ButtonUse->OnClicked.AddDynamic(this, &UItemInventorySlot::UseButtonClicked);
		}
		if (ButtonEquipToQuickSlot)
		{
			ButtonEquipToQuickSlot->OnClicked.AddDynamic(this, &UItemInventorySlot::QuickSlotButtonClicked);
		}
		if (ButtonCancel)
		{
			ButtonCancel->OnClicked.AddDynamic(this, &UItemInventorySlot::CancelButtonClicked);
		}
	}

	if (ItemSlotCountText)
	{
		ItemSlotCountText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UItemInventorySlot::SetItemSlotCountText(int32 Count)
{
	if (ItemSlotCountText)
	{
		ItemSlotCountText->SetText(FText::FromString(FString::FromInt(Count)));
	}
}

void UItemInventorySlot::InitializeIcon()
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
	Brush.SetImageSize(FVector2D(110.f));
	ItemInventorySlotIcon->SetBrush(Brush);
	ItemSlotCountText->SetVisibility(ESlateVisibility::Hidden);
}

void UItemInventorySlot::ActivateItemInventorySlotClick()
{
	if (ItemInventorySlotClick)
	{
		ItemInventorySlotClick->SetIsEnabled(true);
		ItemInventorySlotClick->SetVisibility(ESlateVisibility::Visible);
	}
}

void UItemInventorySlot::DeactivateItemInventorySlotClick()
{
	if (ItemInventorySlotClick)
	{
		ItemInventorySlotClick->SetIsEnabled(false);
		ItemInventorySlotClick->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemInventorySlot::UseButtonClicked()
{
	OnItemMenuSelect.ExecuteIfBound(SlottedItem, EItemSlotMenuState::EISMS_Use);
	DeactivateItemInventorySlotClick();
}

void UItemInventorySlot::QuickSlotButtonClicked()
{
	if (bItemEquippedQuickSlot)
	{
		ButtonEquipToQuickSlot->SetIsEnabled(true);
		ButtonCancel->SetVisibility(ESlateVisibility::Hidden);
		OnItemMenuSelect.ExecuteIfBound(SlottedItem, EItemSlotMenuState::EISMS_Dismount);
	}
	else
	{
		ButtonEquipToQuickSlot->SetIsEnabled(false);
		ButtonCancel->SetVisibility(ESlateVisibility::Visible);
		OnItemMenuSelect.ExecuteIfBound(SlottedItem, EItemSlotMenuState::EISMS_Equip);
	}
}

void UItemInventorySlot::CancelButtonClicked()
{
	OnItemMenuSelect.ExecuteIfBound(SlottedItem, EItemSlotMenuState::EISMS_Cancel);

	ButtonEquipToQuickSlot->SetIsEnabled(true);
	ButtonCancel->SetVisibility(ESlateVisibility::Hidden);

	DeactivateItemInventorySlotClick();
}

void UItemInventorySlot::SetSlottedItem(AItem* Item)
{
	SlottedItem = Item;
}

void UItemInventorySlot::SetItemEquippedQuickSlot(const bool IsEquipped)
{
	bItemEquippedQuickSlot = IsEquipped;
}
