
#include "System/AOSGameModeBase.h"
#include "System/AOSGameInstance.h"
#include "Player/AOSCharacter.h"
#include "Player/AOSAnimInstance.h"
#include "Player/AOSController.h"
#include "EnemyBoss/EnemyBoss.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Components/CombatComponent.h"
#include "Components/ItemComponent.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
#include "HUD/ItemInventorySlot.h"
#include "Sound/SoundCue.h"

AAOSGameModeBase::AAOSGameModeBase()
{
	InitializeTMap();
	InitializeQuickSlotItemArray();
}

void AAOSGameModeBase::RespawnPlayer()
{
	PlayerDeathCount++;

	DPlayerRespawn.Broadcast();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		PlayerPawn->Reset();
		PlayerPawn->Destroy();
	}

	if (PlayerStart == nullptr)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		PlayerStart = Cast<APlayerStart>(PlayerStarts[0]);
	}
	RestartPlayerAtPlayerStart(GetWorld()->GetFirstPlayerController(), PlayerStart);

	AAOSCharacter* Cha = Cast<AAOSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Cha)
	{
		Cha->GetCombatComp()->PlayerDeathDelegate.AddUObject(this, &AAOSGameModeBase::ShowDeathSignWidget);
	}
}

bool AAOSGameModeBase::IsPlayerRespawn()
{
	return PlayerDeathCount > 0;
}

void AAOSGameModeBase::LoadPlayerData()
{
	GI = GetWorld()->GetGameInstance<UAOSGameInstance>();
	if (GI == nullptr)
		return;

	Character = Cast<AAOSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	UAOSAnimInstance* AnimInstance = Cast<UAOSAnimInstance>(Character->GetMesh()->GetAnimInstance());
	CharacterController = Cast<AAOSController>(GetWorld()->GetFirstPlayerController());
	if (Character == nullptr || CharacterController == nullptr)
		return;

	Character->GetCombatComp()->SetController(CharacterController);
	Character->GetCombatComp()->SetGameMode(this);
	Character->GetCombatComp()->SetAnimInstance(AnimInstance);
	Character->GetItemComp()->SetController(CharacterController);
	Character->GetItemComp()->SetGameMode(this);

	LoadPlayerInfo();

	LoadItemInfo();

	LoadWeaponInfo();
}

void AAOSGameModeBase::BindBossDefeatEvent()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyBoss::StaticClass(), Actors);
	if (Actors.Num() == 0)
		return;

	if (Actors[0])
	{
		AEnemyBoss* Boss = Cast<AEnemyBoss>(Actors[0]);
		if (Boss)
		{
			Boss->DBossDefeat.BindUObject(this, &AAOSGameModeBase::ShowBossDefeatedSignWidget);
		}
	}
}

void AAOSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<AAOSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Character)
	{
		Character->GetCombatComp()->PlayerDeathDelegate.AddUObject(this, &AAOSGameModeBase::ShowDeathSignWidget);
	}
}

void AAOSGameModeBase::LoadPlayerInfo()
{
	Character->SetCharacterController();
	Character->GetCombatComp()->SetHealth(GI->GetCurrentHealth());
	Character->GetCombatComp()->SetMana(GI->GetCurrentMana());

	if (GI->GetIsArmed())
	{
		Character->SetWalkingSpeed(EWalkingState::EWS_Armed);
	}
	else
	{
		Character->SetWalkingSpeed(EWalkingState::EWS_UnArmed);
	}
}

void AAOSGameModeBase::LoadWeaponInfo()
{
	if (GI->GetWeaponInfoCount() == 0)
		return;

	for (int32 i = 0; i < GI->GetWeaponInfoCount(); i++)
	{
		FWeaponInfo WeaponInfo = GI->GetWeaponInfo(i);

		AWeapon* SpawnedWeapon = nullptr;
		switch (WeaponInfo.WeaponType)
		{
		case EWeaponType::EWT_ShinbiSword:
			SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(ShinbiSwordClass);
			break;
		case EWeaponType::EWT_Glaive:
			SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(GlaiveClass);
			break;
		case EWeaponType::EWT_AK47:
			SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(AK47Class);
			break;
		case EWeaponType::EWT_Revenent:
			SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(RevenentClass);
			break;
		case EWeaponType::EWT_Wraith:
			SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WraithClass);
			break;
		}

		switch (WeaponInfo.SlotType)
		{
		case ESlotType::EST_Equip:
			EquippedWeapon = SpawnedWeapon;
			break;
		case ESlotType::EST_Quick1:
			QuickSlot1Weapon = SpawnedWeapon;
			break;
		case ESlotType::EST_Quick2:
			QuickSlot2Weapon = SpawnedWeapon;
			break;
		case ESlotType::EST_None:
			break;
		}

		if (WeaponInfo.LoadedAmmo != -1)
		{
			ARangedWeapon* RW = Cast<ARangedWeapon>(SpawnedWeapon);
			RW->SetLoadedAmmo(WeaponInfo.LoadedAmmo);
			if (WeaponInfo.SlotType == ESlotType::EST_Equip)
				Character->SetView(WeaponInfo.WeaponType);
		}

		Character->GetCombatComp()->PickingUpItem(SpawnedWeapon);
	}

	Character->GetCombatComp()->SettingAfterLevelTransition();
}

void AAOSGameModeBase::LoadItemInfo()
{
	if (GI->GetItemInfoCount() == 0)
		return;

	for (int32 i = 0; i < GI->GetItemInfoCount(); i++)
	{
		FItemInfo ItemInfo = GI->GetItemInfo(i);

		AItem* SpawnedItem = nullptr;
		if (ItemInfo.ItemType == EItemType::EIT_Recovery)
		{
			ERecoveryType RT = static_cast<ERecoveryType>(ItemInfo.ItemDetailType);
			if (RT == ERecoveryType::ERT_Health)
			{
				SpawnedItem = GetWorld()->SpawnActor<AItem>(HealthRecoveryClass);
			}
			else if (RT == ERecoveryType::ERT_Mana)
			{
				SpawnedItem = GetWorld()->SpawnActor<AItem>(ManaRecoveryClass);
			}
			else
			{
				SpawnedItem = GetWorld()->SpawnActor<AItem>(StaminaRecoveryClass);
			}

			Character->GetCombatComp()->PickingUpItem(SpawnedItem);
			RecoveryItemMap[RT] = ItemInfo.ItemCount;
			CharacterController->SetItemSlotCountText(i, ItemInfo.ItemCount);

			if (ItemInfo.QuickSlotIndex != -1)
			{
				SpawnedItem->GetItemInventorySlot()->QuickSlotButtonClicked();
				CharacterController->EnrollToItemQuickSlot(ItemInfo.QuickSlotIndex);
			}

			if (ItemInfo.bIsEquipped)
			{
				CharacterController->SetEquippedQuickItem(ItemInfo.QuickSlotIndex, SpawnedItem);
			}
		}
		else if((ItemInfo.ItemType == EItemType::EIT_Ammo))
		{
			EAmmoType AT = static_cast<EAmmoType>(ItemInfo.ItemDetailType);
			if (AT == EAmmoType::EAT_AR)
			{
				SpawnedItem = GetWorld()->SpawnActor<AItem>(ARAmmoClass);
			}
			else if (AT == EAmmoType::EAT_Shell)
			{
				SpawnedItem = GetWorld()->SpawnActor<AItem>(ShotgunAmmoClass);
			}
			else
			{
				SpawnedItem = GetWorld()->SpawnActor<AItem>(SniperAmmoClass);
			}

			Character->GetCombatComp()->PickingUpItem(SpawnedItem);
			AmmoQuantityMap[AT] = ItemInfo.ItemCount;
			CharacterController->SetItemSlotCountText(i, ItemInfo.ItemCount);
		}
	}
}

void AAOSGameModeBase::ShowDeathSignWidget()
{
	if (DeathSignWidgetClass)
	{
		DeathSignWidget = CreateWidget<UUserWidget>(GetWorld(), DeathSignWidgetClass);
		if (DeathSignWidget)
		{
			DeathSignWidget->AddToViewport();
			GetWorldTimerManager().SetTimer(DeathSignTimer, this, &AAOSGameModeBase::DeathSignDestroy, 5.5f);
		}
	}
	if (DeathSound)
	{
		UGameplayStatics::PlaySound2D(this, DeathSound);
	}
}

void AAOSGameModeBase::DeathSignDestroy()
{
	DeathSignWidget->RemoveFromViewport();
}

void AAOSGameModeBase::ShowBossDefeatedSignWidget()
{
	if (BossDefeatedSignWidgetClass)
	{
		BossDefeatedSignWidget = CreateWidget<UUserWidget>(GetWorld(), BossDefeatedSignWidgetClass);
		if (BossDefeatedSignWidget)
		{
			BossDefeatedSignWidget->AddToViewport();
			GetWorldTimerManager().SetTimer(BossDefeatedSignTimer, this, &AAOSGameModeBase::BossDefeatedSignDestroy, 12.f);
		}
	}
	if (BossDefeatedSound)
	{
		UGameplayStatics::PlaySound2D(this, BossDefeatedSound);
	}
}

void AAOSGameModeBase::BossDefeatedSignDestroy()
{
	BossDefeatedSignWidget->RemoveFromViewport();
}

void AAOSGameModeBase::AddWeaponToArr(AWeapon* Weapon)
{
	AcquiredWeapons.Add(Weapon);
}

void AAOSGameModeBase::RemoveWeaponFromArr(AWeapon* Weapon)
{
	AcquiredWeapons.Remove(Weapon);
}

void AAOSGameModeBase::KeepEquippedWeapon(AWeapon* Weapon)
{
	EquippedWeapon = Weapon;
}

void AAOSGameModeBase::KeepQuickSlot1Weapon(AWeapon* Weapon)
{
	QuickSlot1Weapon = Weapon;
}

void AAOSGameModeBase::KeepQuickSlot2Weapon(AWeapon* Weapon)
{
	QuickSlot2Weapon = Weapon;
}

int32 AAOSGameModeBase::GetWeaponCount() const
{
	return AcquiredWeapons.Num();
}

AWeapon* AAOSGameModeBase::GetWeaponItem(int32 Index) const
{
	return AcquiredWeapons[Index];
}

AWeapon* AAOSGameModeBase::GetEquippedWeapon()
{
	return EquippedWeapon;
}

AWeapon* AAOSGameModeBase::GetQuickSlot1Weapon()
{
	return QuickSlot1Weapon;
}

AWeapon* AAOSGameModeBase::GetQuickSlot2Weapon()
{
	return QuickSlot2Weapon;
}

void AAOSGameModeBase::InitializeTMap()
{
	RecoveryItemMap.Add(ERecoveryType::ERT_Health, 0);
	RecoveryItemMap.Add(ERecoveryType::ERT_Mana, 0);
	RecoveryItemMap.Add(ERecoveryType::ERT_Stamina, 0);

	AmmoIndexMap.Add(EAmmoType::EAT_AR, -1);
	AmmoIndexMap.Add(EAmmoType::EAT_Sniper, -1);
	AmmoIndexMap.Add(EAmmoType::EAT_Shell, -1);
}

void AAOSGameModeBase::InitializeQuickSlotItemArray()
{
	for (int8 i = 0; i < 5; i++)
	{
		QuickSlotItemArray.Add(FQuickSlotItem{ nullptr, nullptr, nullptr});
	}
}

AItem* AAOSGameModeBase::GetItem(int32 Index) const
{
	return ItemArray[Index];
}

void AAOSGameModeBase::AddItemToArr(AItem* Item)
{
	ItemArray.Add(Item);
}

void AAOSGameModeBase::RemoveItemFromArr(AItem* Item)
{
	ItemArray.Remove(Item);
}

int32 AAOSGameModeBase::GetItemIndex(AItem* Item) const
{
	return ItemArray.Find(Item);
}

int32 AAOSGameModeBase::GetItemCount() const
{
	return ItemArray.Num();
}

int32 AAOSGameModeBase::GetRecoveryItemCount(ERecoveryType Type) const
{
	return RecoveryItemMap[Type];
}

void AAOSGameModeBase::AddRecoveryItem(ERecoveryType Type, int32 Quantity)
{
	RecoveryItemMap[Type] += Quantity;
}

void AAOSGameModeBase::AddAmmoToAmmoMap(EAmmoType Type, int32 AmmoQuantity)
{
	AmmoQuantityMap.Add(Type, AmmoQuantity);
}

void AAOSGameModeBase::AddAmmoQuantity(EAmmoType Type, int32 AmmoQuantity)
{
	AmmoQuantityMap[Type] += AmmoQuantity;
}

int32 AAOSGameModeBase::GetAmmoQuantity(EAmmoType Type) const
{
	return AmmoQuantityMap[Type];
}

bool AAOSGameModeBase::IsAmmoTypeExist(EAmmoType Type)
{
	return AmmoQuantityMap.Contains(Type);
}

int32 AAOSGameModeBase::GetAmmoIndex(EAmmoType Type) const
{
	return AmmoIndexMap[Type];
}

void AAOSGameModeBase::SetAmmoIndex(EAmmoType Type, int32 Count)
{
	AmmoIndexMap[Type] = Count;
}

AItem* AAOSGameModeBase::GetQuickSlotItem(int8 Index)
{
	return QuickSlotItemArray[Index].QuickSlotItem;
}

void AAOSGameModeBase::SetQuickSlotItem(int32 Index, AItem* Item, UImage* Icon, UTextBlock* CountText)
{
	QuickSlotItemArray[Index].QuickSlotItem = Item;
	QuickSlotItemArray[Index].QuickSlotIcon = Icon;
	QuickSlotItemArray[Index].QuickSlotItemCountText = CountText;
}

void AAOSGameModeBase::SetQuickSlotCountText(int32 Index, FText Text)
{
	QuickSlotItemArray[Index].QuickSlotItemCountText->SetText(Text);
}

void AAOSGameModeBase::ClearQuickSlot(int32 Index)
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
	Brush.SetImageSize(FVector2D(110.f));
	QuickSlotItemArray[Index].QuickSlotIcon->SetBrush(Brush);
	QuickSlotItemArray[Index].QuickSlotItemCountText->SetVisibility(ESlateVisibility::Hidden);
	QuickSlotItemArray[Index] = FQuickSlotItem{ nullptr, nullptr, nullptr };
}

int32 AAOSGameModeBase::GetQuickSlotItemArrLength() const
{
	return QuickSlotItemArray.Num();
}

FQuickSlotItem AAOSGameModeBase::GetQuickSlotItemArr(int32 Index)
{
	return QuickSlotItemArray[Index];
}

int8 AAOSGameModeBase::GetActivatedQuickSlotNumber() const
{
	return ActivatedQuickSlotNumber;
}

void AAOSGameModeBase::SetActivatedQuickSlotNumber(int8 Number)
{
	ActivatedQuickSlotNumber = Number;
}

void AAOSGameModeBase::SetPlayerStart(APlayerStart* RespawnPoint)
{
	PlayerStart = RespawnPoint;
}
