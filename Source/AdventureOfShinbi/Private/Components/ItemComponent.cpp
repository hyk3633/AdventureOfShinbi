

#include "Components/ItemComponent.h"
#include "Components/CombatComponent.h"
#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "HUD/AOSHUD.h"
#include "HUD/AOSCharacterOverlay.h"
#include "HUD/Inventory.h"
#include "HUD/ItemInventorySlot.h"
#include "Items/Item.h"
#include "Items/ItemRecovery.h"
#include "Items/ItemAmmo.h"
#include "Components/Button.h"


UItemComponent::UItemComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeTMap();

	CharacterController = Cast<AAOSController>(Character->GetController());
	if (CharacterController)
	{
		HUD = Cast<AAOSHUD>(CharacterController->GetHUD());
	}
}

void UItemComponent::AddItem(AItem* Item)
{
	if (ItemArray.Num() % 5 == 0)
	{
		HUD->CreateItemInventorySlot();
	}

	switch (Item->GetItemType())
	{
	case EItemType::EIT_Recovery:
		AddRecoveryItem(Item);
		break;
	case EItemType::EIT_Ammo:
		AddAmmoItem(Item);
		break;
	}
}

void UItemComponent::AddRecoveryItem(AItem* Item)
{
	AItemRecovery* Recovery = Cast<AItemRecovery>(Item);

	if (RecoveryItemMap[Recovery->GetRecoveryType()] == 0)
	{
		ItemArray.Add(Item);
		HUD->AddItemToSlot(ItemArray.Num()-1,Item);
		HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num()-1]->OnItemUse.BindUObject(this, &UItemComponent::ItemUse);
		Recovery->GetStaticMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Recovery->GetStaticMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Recovery->GetStaticMesh()->SetVisibility(false);
	}
	else
	{
		Item->Destroy();
	}

	RecoveryItemMap[Recovery->GetRecoveryType()] += 1;

	HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num()-1]->SetItemSlotCountText(RecoveryItemMap[Recovery->GetRecoveryType()]);
}

void UItemComponent::AddAmmoItem(AItem* Item)
{
	AItemAmmo* Ammo = Cast<AItemAmmo>(Item);

	if (AmmoQuantityMap[Ammo->GetAmmoType()] == 0)
	{
		ItemArray.Add(Item);
		HUD->AddItemToSlot(ItemArray.Num()-1, Item);
		HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num() - 1]->ItemInventorySlotIconButton->SetIsEnabled(false);
		HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num()-1]->OnItemUse.BindUObject(this, &UItemComponent::ItemUse);
		Ammo->GetStaticMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Ammo->GetStaticMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Ammo->GetStaticMesh()->SetVisibility(false);
	}
	else
	{
		Item->Destroy();
	}

	AmmoQuantityMap[Ammo->GetAmmoType()] += Ammo->GetAmmoQuantity();

	HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Num()-1]->SetItemSlotCountText(AmmoQuantityMap[Ammo->GetAmmoType()]);

}

void UItemComponent::ItemUse(AItem* Item)
{
	switch (Item->GetItemType())
	{
	case EItemType::EIT_Recovery:
		UseRecoveryItem(Item);
		break;
	case EItemType::EIT_Ammo:
		AddAmmoItem(Item);
		break;
	}
}

void UItemComponent::UseRecoveryItem(AItem* Item)
{
	AItemRecovery* IR = Cast<AItemRecovery>(Item);
	if(IR)
	{
		if (RecoveryItemMap[IR->GetRecoveryType()] > 0)
		{
			RecoveryItemMap[IR->GetRecoveryType()] -= 1;
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(Item)]->SetItemSlotCountText(RecoveryItemMap[IR->GetRecoveryType()]);
			if (IR->GetRecoveryType() == ERecoveryType::ERT_Health)
			{
				bDoRecoveryHealth = true;
			}
			else if (IR->GetRecoveryType() == ERecoveryType::ERT_Mana)
			{

			}
			else if (IR->GetRecoveryType() == ERecoveryType::ERT_Stamina)
			{

			}
		}

		if (RecoveryItemMap[IR->GetRecoveryType()] == 0)
		{
			UItemInventorySlot* Slot = HUD->CharacterOverlay->InventoryWidget->ItemSlotArray[ItemArray.Find(Item)];
			Slot->InitializeIcon();
			Slot->SetSlottedItem(nullptr);
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray.Remove(Slot);
			HUD->CharacterOverlay->InventoryWidget->ItemSlotArray.Add(Slot);
			HUD->UpdateItemInventory();
			ItemArray.Remove(Item);
		}
	}
}

void UItemComponent::RecoveryHealth(float DeltaTime)
{
	if (bDoRecoveryHealth)
	{
		float Increase = HealthRecoveryRate * DeltaTime;
		RecoveredHealthAmount += Increase;
		if (RecoveredHealthAmount <= HealthRecoveryAmount)
		{
			CombatComp->Health = FMath::Clamp(CombatComp->Health + Increase, 0.f, CombatComp->MaxHealth);
			CharacterController->SetHUDHealthBar(CombatComp->Health, CombatComp->MaxHealth);
		}
		else
		{
			bDoRecoveryHealth = false;
			RecoveredHealthAmount = 0.f;
		}
	}
}

void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RecoveryHealth(DeltaTime);
}

void UItemComponent::InitializeTMap()
{
	RecoveryItemMap.Add(ERecoveryType::ERT_Health, 0);
	RecoveryItemMap.Add(ERecoveryType::ERT_Mana, 0);
	RecoveryItemMap.Add(ERecoveryType::ERT_Stamina, 0);

	AmmoQuantityMap.Add(EAmmoType::EAT_AR, 0);
	AmmoQuantityMap.Add(EAmmoType::EAT_SMG, 20);
	AmmoQuantityMap.Add(EAmmoType::EAT_Pistol, 0);
	AmmoQuantityMap.Add(EAmmoType::EAT_Shell, 0);
	AmmoQuantityMap.Add(EAmmoType::EAT_GrenadeLauncher, 0);
	AmmoQuantityMap.Add(EAmmoType::EAT_Rocket, 0);
}

void UItemComponent::SetCharacter(AAOSCharacter* CharacterToSet)
{
	Character = CharacterToSet;
}

void UItemComponent::SetCombatComp(UCombatComponent* Combat)
{
	CombatComp = Combat;
}

TMap<EAmmoType, int32> UItemComponent::GetAmmoMap() const
{
	return AmmoQuantityMap;
}

