

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

	{
		const int8 Idx = GameMode->GetRecoveryIndex(ERecoveryType::ERT_Health);
		if (Idx != -1)
		{
			AItemRecovery* Recovery = Cast<AItemRecovery>(GameMode->GetItem(Idx));
			HealthRecoveryAmount = Recovery->GetRecoveryQuantity();
		}
	}

	{
		const int8 Idx = GameMode->GetRecoveryIndex(ERecoveryType::ERT_Mana);
		if (Idx != -1)
		{
			AItemRecovery* Recovery = Cast<AItemRecovery>(GameMode->GetItem(Idx));
			ManaRecoveryAmount = Recovery->GetRecoveryQuantity();
		}
	}

	{
		const int8 Idx = GameMode->GetRecoveryIndex(ERecoveryType::ERT_Stamina);
		if (Idx != -1)
		{
			AItemRecovery* Recovery = Cast<AItemRecovery>(GameMode->GetItem(Idx));
			StaminaRecoveryBoostAmount = Recovery->GetRecoveryQuantity();
		}
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

		GameMode->SetRecoveryIndex(Recovery->GetRecoveryType(), GameMode->GetItemCount() - 1);

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
	CharacterController->SetItemSlotCountText(GameMode->GetRecoveryIndex(Recovery->GetRecoveryType()), RecoveryItemCount);
	CharacterController->UpdateQuickSlotItemText(GameMode->GetRecoveryIndex(Recovery->GetRecoveryType()), RecoveryItemCount);
}

void UItemComponent::AddAmmoItem(AItem* Item)
{
	AItemAmmo* Ammo = Cast<AItemAmmo>(Item);
	Ammo->HandleItemAfterGain();

	// 탄약이 배열에 존재하면 탄약량만 추가
	if (GameMode->IsAmmoTypeExist(Ammo->GetAmmoType()))
	{
		GameMode->AddAmmoQuantity(Ammo->GetAmmoType(), Ammo->GetAmmoQuantity());
		Item->Destroy();
	}
	else // 탄약이 배열에 존재하지 않으면 탄약 아이템 슬롯에 추가
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
	
	// 현재 장착된 무기가 원거리 무기고 무기의 탄약 타입과 획득한 탄약의 타입이 같으면 탄약 정보 HUD 갱신
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

// 아이템을 슬롯에서 제거
void UItemComponent::DismountItem(AItem* Item)
{
	const int32 ItemIndex = GameMode->GetItemIndex(Item);

	CharacterController->SetHUDItemSlotDismount(ItemIndex);

	AItemRecovery* IR = Cast<AItemRecovery>(Item);
	if (IR == nullptr)
		return;
	
	// 퀵슬롯에 등록된 아이템이면 해당 퀵슬롯 초기화
	const int8 QuickIdx = IR->GetQuickSlotIndex();
	if (QuickIdx != -1)
	{
		GameMode->ClearQuickSlot(QuickIdx);

		CharacterController->DeactivateItemInventorySlotClick(ItemIndex);

		if (GameMode->GetActivatedQuickSlotNumber() == QuickIdx)
		{
			CharacterController->ClearEquippedItemSlotHUD();
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

			// 퀵슬롯에 등록된 아이템이면 해당 퀵슬롯의 아이템 개수 텍스트 갱신
			const int8 QuickIdx = IR->GetQuickSlotIndex();
			if (QuickIdx != -1)
			{
				const FText RecoveryItemCountText = FText::FromString(FString::FromInt(RecoveryItemCount));
				GameMode->SetQuickSlotCountText(QuickIdx, RecoveryItemCountText);

				if (GameMode->GetActivatedQuickSlotNumber() == QuickIdx)
				{
					CharacterController->SetEquippedItemSlotCountText(RecoveryItemCountText);
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

		// 아이템을 모두 소모했다면 슬롯에서 아이템 제거
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
		// 체력 회복 불가일 경우
		if (CombatComp->GetHealBanActivated())
		{
			CombatComp->Health = FMath::FloorToFloat(CombatComp->Health);
			bDoRecoveryHealth = false;
			RecoveredHealthAmount = 0.f;

			return;
		}

		float Increase = HealthRecoveryRate * DeltaTime;
		RecoveredHealthAmount += Increase;

		// 회복된 체력량이 회복할 양보다 적으면 계속 회복
		if (RecoveredHealthAmount <= HealthRecoveryAmount)
		{
			CombatComp->Health = FMath::Clamp(CombatComp->Health + Increase, 0.f, CombatComp->MaxHealth);
			CharacterController->SetHUDHealthBar(CombatComp->Health, CombatComp->MaxHealth);
		}
		else // 회복량만큼을 회복했으면 회복 중단
		{
			bDoRecoveryHealth = false;
			RecoveredHealthAmount = 0.f;

			// 낮은 체력 목소리가 재생된 경우 회복 후 목소리 재생
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

			// 마나가 없는데 사용하려 했을 경우 마나 부족 목소리 출력
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

