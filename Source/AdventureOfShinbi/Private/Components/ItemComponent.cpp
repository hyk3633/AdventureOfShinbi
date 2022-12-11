

#include "Components/ItemComponent.h"
#include "Components/CombatComponent.h"
#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "System/AOSGameModeBase.h"
#include "Items/Item.h"
#include "Items/ItemRecovery.h"
#include "Items/ItemAmmo.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterController = Character->GetController<AAOSController>();
	
	GameMode = GetWorld()->GetAuthGameMode<AAOSGameModeBase>();

	if (CombatComp)
	{
		DefaultStaminaIncreaseRate = CombatComp->StaminaIncreaseRate;
	}
}

void UItemComponent::RestartItemComp()
{
	GameMode = GetWorld()->GetAuthGameMode<AAOSGameModeBase>();
	CharacterController = Character->GetController<AAOSController>();
	if (GameMode && CharacterController)
	{
		for (int32 i = 0; i < GameMode->GetItemCount(); i++)
		{
			if (GameMode->GetItem(i)->GetItemType() == EItemType::EIT_Recovery)
			{
				CharacterController->BindToItemSlot(i);
			}
		}
		CharacterController->SetQuickSlotItemAuto();
	}
}

void UItemComponent::AddItem(AItem* Item)
{
	CharacterController->CreateHUDItemInventorySlot();

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

void UItemComponent::UseActivatedQuickSlotItem()
{
	if (GameMode->GetQuickSlotItem(GameMode->GetActivatedQuickSlotNumber()))
	{
		UseItem(GameMode->GetQuickSlotItem(GameMode->GetActivatedQuickSlotNumber()));
	}
}

void UItemComponent::AddRecoveryItem(AItem* Item)
{
	AItemRecovery* Recovery = Cast<AItemRecovery>(Item);
	Recovery->HandleItemAfterGain();

	if (GameMode->GetRecoveryItemCount(Recovery->GetRecoveryType()) == 0)
	{
		GameMode->AddItemToArr(Item);

		CharacterController->AddItemToSlot(Item);
		CharacterController->BindToItemSlot(GameMode->GetItemCount() - 1);

		if (Recovery->GetRecoveryType() == ERecoveryType::ERT_Health)
		{
			HealthRecoveryAmount = Recovery->GetRecoveryQuantity();
		}
		else if (Recovery->GetRecoveryType() == ERecoveryType::ERT_Mana)
		{
			ManaRecoveryAmount = Recovery->GetRecoveryQuantity();
		}
		else
		{
			StaminaRecoveryBoostAmount = Recovery->GetRecoveryQuantity();
		}
	}
	else
	{
		Item->Destroy();
	}

	GameMode->AddRecoveryItem(Recovery->GetRecoveryType(), 1);

	const int32 RecoveryItemCount = GameMode->GetRecoveryItemCount(Recovery->GetRecoveryType());
	CharacterController->SetItemSlotCountText(GameMode->GetItemCount() - 1, RecoveryItemCount);
}

void UItemComponent::AddAmmoItem(AItem* Item)
{
	AItemAmmo* Ammo = Cast<AItemAmmo>(Item);
	Ammo->HandleItemAfterGain();

	if (GameMode->IsAmmoTypeExist(Ammo->GetAmmoType()))
	{
		GameMode->AddAmmoQuantity(Ammo->GetAmmoType(), Ammo->GetAmmoQuantity());
		Item->Destroy();
	}
	else
	{
		GameMode->AddItemToArr(Item);
		GameMode->AddAmmoToAmmoMap(Ammo->GetAmmoType(), Ammo->GetAmmoQuantity());
		GameMode->SetAmmoIndex(Ammo->GetAmmoType(), GameMode->GetItemCount() - 1);

		CharacterController->AddItemToSlot(Item);
		CharacterController->DisableItemSlotButton();
	}

	const int32 AmmoQuantity = GameMode->GetAmmoQuantity(Ammo->GetAmmoType());
	const int32 AmmoIndex = GameMode->GetAmmoIndex(Ammo->GetAmmoType());
	CharacterController->SetItemSlotCountText(AmmoIndex, AmmoQuantity);
	
	// 현재 장착된 무기가 총이고 총의 탄약 타입과 획득한 탄약의 타입이 같으면 탄약 정보 ui 갱신
	if (CombatComp->GetEquippedWeapon())
	{
		ARangedWeapon* RW = Cast<ARangedWeapon>(CombatComp->GetEquippedWeapon());
		if (RW)
		{
			if (RW->GetAmmoType() == Ammo->GetAmmoType())
			{
				CharacterController->SetHUDTotalAmmoText(AmmoQuantity);
			}
		}
	}
}

void UItemComponent::ItemUseOrEquip(AItem* Item, EItemSlotMenuState State)
{
	if (State == EItemSlotMenuState::EISMS_Use)
	{
		UseItem(Item);
	}
	else if(State == EItemSlotMenuState::EISMS_Equip)
	{
		CharacterController->SetSelectedItem(Item);
		CharacterController->AllHUDQuickSlotButtonEnabled();
	}
	else if (State == EItemSlotMenuState::EISMS_Cancel)
	{
		CharacterController->AllHUDQuickSlotButtonDisabled();
	}
	else if (State == EItemSlotMenuState::EISMS_Dismount)
	{
		DismountItem(Item);
	}
}

void UItemComponent::UseItem(AItem* Item)
{
	switch (Item->GetItemType())
	{
	case EItemType::EIT_Recovery:
		UseRecoveryItem(Item);
		break;
	}
}

void UItemComponent::DismountItem(AItem* Item)
{
	const int32 ItemIndex = GameMode->GetItemIndex(Item);

	CharacterController->SetHUDItemSlotDismount(ItemIndex);
	
	for (int8 i = 0; i < 5; i++)
	{
		if (GameMode->GetQuickSlotItem(i) == Item)
		{
			GameMode->ClearQuickSlot(i);

			CharacterController->DeactivateItemInventorySlotClick(ItemIndex);

			if (GameMode->GetActivatedQuickSlotNumber() == i)
			{
				CharacterController->ClearEquippedItemSlotHUD();
			}
		}
	}
}

void UItemComponent::PlayQuickSlotAnimation()
{
	CharacterController->PlayQuickSlotActivateAnimation();
}

void UItemComponent::UseRecoveryItem(AItem* Item)
{
	if (CombatComp->GetHealBanActivated()) 
		return;

	AItemRecovery* IR = Cast<AItemRecovery>(Item);
	if(IR)
	{
		const int32 ItemIndex = GameMode->GetItemIndex(Item);

		IR->PlayUsingEffect(Character->GetActorLocation());

		if (GameMode->GetRecoveryItemCount(IR->GetRecoveryType()) > 0)
		{
			GameMode->AddRecoveryItem(IR->GetRecoveryType(), -1);
			const int32 RecoveryItemCount = GameMode->GetRecoveryItemCount(IR->GetRecoveryType());

			CharacterController->SetItemSlotCountText(ItemIndex, RecoveryItemCount);

			for (int8 i = 0; i < 5; i++)
			{
				if (GameMode->GetQuickSlotItem(i) == Item)
				{
					const FText RecoveryItemCountText = FText::FromString(FString::FromInt(RecoveryItemCount));
					GameMode->SetQuickSlotCountText(i, RecoveryItemCountText);

					if (GameMode->GetActivatedQuickSlotNumber() == i)
					{
						CharacterController->SetEquippedItemSlotCountText(RecoveryItemCountText);
					}
				}
			}

			if (IR->GetRecoveryType() == ERecoveryType::ERT_Health)
			{
				bDoRecoveryHealth = true;
			}
			else if (IR->GetRecoveryType() == ERecoveryType::ERT_Mana)
			{
				bDoRecoveryMana = true;
			}
			else if (IR->GetRecoveryType() == ERecoveryType::ERT_Stamina)
			{
				CombatComp->StaminaIncreaseRate *= StaminaRecoveryBoostAmount;
				Character->GetWorldTimerManager().SetTimer(StaminaRecoveryBoostTimer, this, &UItemComponent::StaminaRecoveryBoostTimeOff, StaminaRecoveryBoostTime);
			}
		}

		if (GameMode->GetRecoveryItemCount(IR->GetRecoveryType()) == 0)
		{
			// 만일 퀵슬롯에 아이템이 장착되어 있으면 퀵슬롯 비우기
			DismountItem(Item);

			CharacterController->UpdateItemInventory(ItemIndex);
			GameMode->RemoveItemFromArr(Item);
		}
	}
}

void UItemComponent::Recovery(float DeltaTime)
{
	if (bDoRecoveryHealth)
	{
		if (CombatComp->GetHealBanActivated())
		{
			CombatComp->Health = FMath::FloorToFloat(CombatComp->Health);
			bDoRecoveryHealth = false;
			RecoveredHealthAmount = 0.f;
		}

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
			if (VoiceRecoveryHealth && CombatComp->bVoiceLowHealthPlayed)
			{
				UGameplayStatics::PlaySound2D(this, VoiceRecoveryHealth);
				CombatComp->bVoiceLowHealthPlayed = false;
			}
		}
	}
	if (bDoRecoveryMana)
	{
		float Increase = ManaRecoveryRate * DeltaTime;
		RecoveredManaAmount += Increase;
		if (RecoveredManaAmount <= ManaRecoveryAmount)
		{
			CombatComp->Mana = FMath::Clamp(CombatComp->Mana + Increase, 0.f, CombatComp->MaxMana);
			CharacterController->SetHUDManaBar(CombatComp->Mana, CombatComp->MaxMana);
		}
		else
		{
			bDoRecoveryMana = false;
			RecoveredManaAmount = 0.f;
			if (CombatComp->bVoiceLackManaPlayed)
			{
				CombatComp->bVoiceLackManaPlayed = false;
			}
		}
	}
}

int32 UItemComponent::GetItemCount(AItem* Item)
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

void UItemComponent::StaminaRecoveryBoostTimeOff()
{
	CombatComp->StaminaIncreaseRate = DefaultStaminaIncreaseRate;
}

void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Recovery(DeltaTime);
}

void UItemComponent::SetCharacter(AAOSCharacter* CharacterToSet)
{
	Character = CharacterToSet;
}

void UItemComponent::SetCombatComp(UCombatComponent* Combat)
{
	CombatComp = Combat;
}

void UItemComponent::SetController(AAOSController* Cont)
{
	CharacterController = Cont;
}

void UItemComponent::SetGameMode(AAOSGameModeBase* Gamemode)
{
	GameMode = Gamemode;
}

