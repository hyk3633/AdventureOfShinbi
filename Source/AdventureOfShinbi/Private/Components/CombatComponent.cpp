

#include "Components/CombatComponent.h"
#include "Components/ItemComponent.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/Item.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
#include "Weapons/RangedHitScanWeapon.h"
#include "Weapons/RangedProjectileWeapon.h"
#include "HUD/AOSHUD.h"
#include "HUD/AOSCharacterOverlay.h"
#include "HUD/Inventory.h"
#include "HUD/InventorySlot.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Types/WeaponState.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::InitializeComponent()
{
	Super::InitializeComponent();

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterController = Cast<AAOSController>(Character->GetController());
	if (CharacterController)
	{
		HUD = Cast<AAOSHUD>(CharacterController->GetHUD());
		CharacterController->SetHUDHealthBar(Health, MaxHealth);
		CharacterController->SetHUDStaminaBar(Stamina, MaxStamina);
	}

	DefaultFOV = Character->GetCamera()->FieldOfView;
	CurrentFOV = DefaultFOV;
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Zoom(DeltaTime);

	UpdateStamina(DeltaTime);

	SpreadCrosshair(DeltaTime);
}

void UCombatComponent::MeleeAttack()
{
	if (Character->GetIsAnimationPlaying()) return;

	if (EquippedWeapon)
	{
		switch (EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_MeleeOneHand:
			Character->GetWorldTimerManager().ClearTimer(ComboTimer);
			Character->GetWorldTimerManager().SetTimer(ComboTimer, this, &UCombatComponent::ResetCombo, ComboTime);
			PlayMontageOneHandAttack();
			break;
		case EWeaponType::EWT_MeleeTwoHand:
			PlayMontageTwoHandAttack();
			break;
		case EWeaponType::EWT_Glave:
			PlayMontageGlaveAttack();
			break;
		}
	}
}

void UCombatComponent::GunFire()
{
	PlayMontageGunFire();
	RangedWeaponFire();
}

void UCombatComponent::PlayMontageOneHandAttack()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || MeleeOneHandAttackMontage == nullptr) return;

	AnimInstance->Montage_Play(MeleeOneHandAttackMontage);
	if (Character->GetCharacterMovement()->IsFalling())
	{
		AnimInstance->Montage_JumpToSection(FName("OneHandAir"));
	}
	else
	{
		if (MeleeAttackComboCount == 0)
		{
			RandomMontageNum = UKismetMathLibrary::RandomInteger(2);
			if (RandomMontageNum == 0)
			{
				AnimInstance->Montage_JumpToSection(FName("OneHandC2"));
			}
			else
			{
				AnimInstance->Montage_JumpToSection(FName("OneHandD"));
			}
			MeleeAttackComboCount++;
		}
		else if (MeleeAttackComboCount == 1)
		{
			if (RandomMontageNum == 0)
			{
				AnimInstance->Montage_JumpToSection(FName("OneHandD"));
			}
			else
			{
				AnimInstance->Montage_JumpToSection(FName("OneHandC2"));
			}
			MeleeAttackComboCount++;
		}
		else if (MeleeAttackComboCount == 2)
		{
			if (RandomMontageNum == 0)
			{
				AnimInstance->Montage_JumpToSection(FName("OneHandC"));
			}
			else
			{
				AnimInstance->Montage_JumpToSection(FName("OneHandB"));
			}
			MeleeAttackComboCount++;
		}
		else if (MeleeAttackComboCount == 3)
		{
			if (RandomMontageNum == 0)
			{
				AnimInstance->Montage_JumpToSection(FName("OneHandB"));
			}
			else
			{
				AnimInstance->Montage_JumpToSection(FName("OneHandC"));
			}
			ResetCombo();
		}
	}
}

void UCombatComponent::PlayMontageTwoHandAttack()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || MeleeTwoHandAttackMontage == nullptr) return;

	AnimInstance->Montage_Play(MeleeTwoHandAttackMontage);
	if (Character->GetCharacterMovement()->IsFalling())
	{
		//AnimInstance->Montage_JumpToSection(FName("MeleeOneHandAir"));
	}
	else
	{
		AnimInstance->Montage_JumpToSection(FName("TwoHandA"));
	}
}

void UCombatComponent::PlayMontageGunFire()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || GunFireMontage == nullptr) return;

	AnimInstance->Montage_Play(GunFireMontage);
	AnimInstance->Montage_JumpToSection(FName("GunFireFast"));
}

void UCombatComponent::PlayMontageGlaveAttack()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || GlaveAttackMontage == nullptr) return;

	AnimInstance->Montage_Play(GlaveAttackMontage);
	if (Character->GetCharacterMovement()->IsFalling())
	{
		AnimInstance->Montage_JumpToSection(FName(""));
	}
	else
	{
		AnimInstance->Montage_JumpToSection(FName("GlaveB"));
	}
}

void UCombatComponent::RangedWeaponFire()
{
	ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);

	if (RangedWeapon->GetLoadedAmmo() > 0)
	{
		RangedWeapon->ConsumeAmmo();
	}

	ERangedWeaponType ERWT = RangedWeapon->GetRangedWeaponType();
	if (ERWT == ERangedWeaponType::ERWT_HitScan)
	{
		ARangedHitScanWeapon* HitScanWeapon = Cast<ARangedHitScanWeapon>(EquippedWeapon);
		HitScanWeapon->Firing();
	}
	else
	{
		ARangedProjectileWeapon* ProjectileWeapon = Cast<ARangedProjectileWeapon>(EquippedWeapon);
		ProjectileWeapon->GetScatterGun() == true ? ProjectileWeapon->ScatterFiring() : ProjectileWeapon->Firing();
	}

	if (CharacterController)
	{
		CharacterController->SetHUDLoadedAmmoText(RangedWeapon->GetLoadedAmmo());
	}

	if (RangedWeapon->GetLoadedAmmo() == 0 && Character->GetItemComp()->GetAmmoMap()[RangedWeapon->GetAmmoType()] > 0)
	{
		Reload();
	}
}

void UCombatComponent::ResetCombo()
{
	MeleeAttackComboCount = 0;
}

void UCombatComponent::Zoom(float DeltaTime)
{
	if (Character == nullptr || EquippedWeapon == nullptr || EquippedWeapon->GetWeaponType() != EWeaponType::EWT_Gun) return;

	ARangedWeapon* RW = Cast<ARangedWeapon>(EquippedWeapon);
	if (RW)
	{
		if (Character->GetIsAiming())
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV, RW->GetZoomScope(), DeltaTime, ZoomSpeed);
		}
		else
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomSpeed);
		}

		if (Character->GetCamera())
		{
			Character->GetCamera()->FieldOfView = CurrentFOV;
		}
	}
}

void UCombatComponent::Reload()
{
	if (Character->GetItemComp() == nullptr) return;

	ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);

	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Gun)
	{
		if (Character->GetItemComp()->GetAmmoMap()[RangedWeapon->GetAmmoType()] == 0) return;

		int32 AmmoToReload = RangedWeapon->GetMagazine() - RangedWeapon->GetLoadedAmmo();
		if (AmmoToReload < Character->GetItemComp()->GetAmmoMap()[RangedWeapon->GetAmmoType()])
		{
			RangedWeapon->SetLoadedAmmo(AmmoToReload);
			Character->GetItemComp()->GetAmmoMap()[RangedWeapon->GetAmmoType()] -= AmmoToReload;
		}
		else
		{
			RangedWeapon->SetLoadedAmmo(Character->GetItemComp()->GetAmmoMap()[RangedWeapon->GetAmmoType()]);
			Character->GetItemComp()->GetAmmoMap()[RangedWeapon->GetAmmoType()] = 0;
		}
	}

	if (CharacterController)
	{
		CharacterController->SetHUDLoadedAmmoText(RangedWeapon->GetLoadedAmmo());
		CharacterController->SetHUDTotalAmmoText(Character->GetItemComp()->GetAmmoMap()[RangedWeapon->GetAmmoType()]);
	}
}

void UCombatComponent::UpdateHealth(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	if (CharacterController)
	{
		CharacterController->SetHUDHealthBar(Health, MaxHealth);
	}
}

void UCombatComponent::UpdateStamina(float DeltaTime)
{
	if (FMath::FloorToFloat(Stamina) > 0.f && bCanRunning)
	{
		if (Character->GetIsRunning())
		{
			float Decrease = DeltaTime * StaminaDecreaseRate;
			Stamina = FMath::Clamp(Stamina - Decrease, 0.f, MaxStamina);
		}
		else
		{
			float Increase = DeltaTime * StaminaIncreaseRate;
			Stamina = FMath::Clamp(Stamina + Increase, 0.f, MaxStamina);
		}
	}
	else
	{
		if (FMath::FloorToFloat(Stamina) == 0.f)
		{
			bCanRunning = false;
			Character->SetCanRunning(false);
			Character->StopRunning();
		}
		if (Stamina <= MaxStamina * 0.6f)
		{
			float Increase = DeltaTime * StaminaIncreaseRate;
			Stamina = FMath::Clamp(Stamina + Increase, 0.f, MaxStamina);
		}
		else
		{
			bCanRunning = true;
			Character->SetCanRunning(true);
			if (Character->GetIsRunning())
			{
				Character->ResumeRunning();
			}
		}
	}

	if (CharacterController)
	{
		CharacterController->SetHUDStaminaBar(Stamina, MaxStamina);
	}
}

void UCombatComponent::SetCrosshair()
{
	if (CharacterController == nullptr) return;

	if (HUD)
	{
		if (EquippedWeapon && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Gun)
		{
			ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);
			if (RangedWeapon)
			{
				HUD->CrosshairCenter = RangedWeapon->CrosshairCenter;
				HUD->CrosshairLeft = RangedWeapon->CrosshairLeft;
				HUD->CrosshairRight = RangedWeapon->CrosshairRight;
				HUD->CrosshairTop = RangedWeapon->CrosshairTop;
				HUD->CrosshairBottom = RangedWeapon->CrosshairBottom;
			}
		}
		else
		{
			HUD->CrosshairCenter = nullptr;
			HUD->CrosshairLeft = nullptr;
			HUD->CrosshairRight = nullptr;
			HUD->CrosshairTop = nullptr;
			HUD->CrosshairBottom = nullptr;
		}
	}
}

void UCombatComponent::SpreadCrosshair(float DeltaTime)
{
	FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
	FVector2D VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	HUD->SetCrosshairSpread(CrosshairVelocityFactor + CrosshairInAirFactor);
}

void UCombatComponent::PickingUpItem(AItem* PickedItem)
{
	if (HUD == nullptr || PickedItem == nullptr) return;

	if (PickedItem->GetIsWeapon())
	{
		AWeapon* PickedWeapon = Cast<AWeapon>(PickedItem);

		if (AcquiredWeapons.Num() % 5 == 0)
		{
			HUD->CreateInventorySlot();
		}

		AcquiredWeapons.Add(PickedWeapon);
		PickedWeapon->SetWeaponState(EWeaponState::EWS_PickedUp);
		PickedWeapon->WeaponStateChanged.AddUObject(this, &UCombatComponent::OnChangedWeaponState);
		HUD->AddWeaponToSlot(AcquiredWeapons.Num() - 1, AcquiredWeapons.Last());
	}
	else
	{
		// TODO : Item ½Àµæ Ã³¸®
		Character->GetItemComp()->AddItem(PickedItem);
	}
}

void UCombatComponent::WeaponQuickSwap()
{
	if (QuickSlot1Weapon == nullptr && QuickSlot2Weapon == nullptr)
	{
		return;
	}
	else if (QuickSlot1Weapon != nullptr && QuickSlot2Weapon == nullptr)
	{
		QuickSlot1Weapon->GetInventorySlot()->EquipButtonClicked();
	}
	else if (QuickSlot1Weapon == nullptr && QuickSlot2Weapon != nullptr)
	{
		QuickSlot2Weapon->GetInventorySlot()->EquipButtonClicked();
	}
	else
	{
		QuickSlot1Weapon->GetInventorySlot()->EquipButtonClicked();
		QuickSlot2Weapon->GetInventorySlot()->QuickSlot1ButtonClicked();
	}
}

void UCombatComponent::OnChangedWeaponState(AWeapon* Weapon)
{
	switch (Weapon->GetWeaponState())
	{
	case EWeaponState::EWS_Equipped:
		EquipWeapon(Weapon);
		break;
	case EWeaponState::EWS_PickedUp:
		UnEquipWeapon(Weapon);
		break;
	case EWeaponState::EWS_QuickSlot1:
		WeaponToQuickSlot1(Weapon);
		break;
	case EWeaponState::EWS_QuickSlot2:
		WeaponToQuickSlot2(Weapon);
		break;
	case EWeaponState::EWS_Field:
		DiscardWeapon(Weapon);
		break;
	}
}

void UCombatComponent::EquipWeapon(AWeapon* Weapon)
{
	AWeapon* WeaponToChange = EquippedWeapon;

	EquippedWeapon = Weapon;

	SetCrosshair();

	FName SocketName;
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Gun)
	{
		ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);
		CharacterController->SetHUDLoadedAmmoText(RangedWeapon->GetLoadedAmmo());
		CharacterController->SetHUDTotalAmmoText(Character->GetItemComp()->GetAmmoMap()[RangedWeapon->GetAmmoType()]);
		CharacterController->HUDAmmoInfoOn();

		if (RangedWeapon->GetLoadedAmmo() == 0 && Character->GetItemComp()->GetAmmoMap()[RangedWeapon->GetAmmoType()] > 0)
		{
			Reload();
		}

		ARangedWeapon* RW = Cast<ARangedWeapon>(EquippedWeapon);
		Character->SetGunRecoil(RW->GetGunRecoil());
		SocketName = FName("GunSocket");
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Bow)
	{
		CharacterController->HUDAmmoInfoOff();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_MeleeOneHand)
	{
		SocketName = FName("OneHandSocket");
		CharacterController->HUDAmmoInfoOff();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_MeleeTwoHand)
	{
		SocketName = FName("TwoHandSocket");
		CharacterController->HUDAmmoInfoOff();
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Glave)
	{
		SocketName = FName("GlaveSocket");
		CharacterController->HUDAmmoInfoOff();
	}

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	CharacterController->SetHUDEquippedWeaponIcon(EquippedWeapon->GetItemIcon());
	CharacterController->SetHUDInventoryEquippedWeaponSlotIcon(EquippedWeapon->GetItemIcon());

	EquippedWeapon->SetOwner(Character);
	Character->SetWeaponType(EquippedWeapon->GetWeaponType());

	if (WeaponToChange != nullptr)
	{
		if (Weapon == QuickSlot1Weapon)
		{
			QuickSlot1Weapon = nullptr;
			WeaponToChange->GetInventorySlot()->QuickSlot1ButtonClicked();
		}
		else if (Weapon == QuickSlot2Weapon)
		{
			QuickSlot2Weapon = nullptr;
			WeaponToChange->GetInventorySlot()->QuickSlot2ButtonClicked();
		}
		else
		{
			WeaponToChange->GetInventorySlot()->EquipButtonClicked();
		}
	}
	else
	{
		if (Weapon == QuickSlot1Weapon)
		{
			QuickSlot1Weapon = nullptr;
			CharacterController->SetHUDWeaponQuickSlot1Icon(nullptr);
			CharacterController->SetHUDInventoryQuickSlot1Icon(nullptr);
		}
		else if (Weapon == QuickSlot2Weapon)
		{
			QuickSlot2Weapon = nullptr;
			CharacterController->SetHUDWeaponQuickSlot2Icon(nullptr);
			CharacterController->SetHUDInventoryQuickSlot2Icon(nullptr);
		}
	}
}

void UCombatComponent::UnEquipWeapon(AWeapon* Weapon)
{
	if (Weapon == EquippedWeapon)
	{
		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachRules);
		EquippedWeapon->SetOwner(nullptr);
		EquippedWeapon = nullptr;
		SetCrosshair();
		CharacterController->SetHUDEquippedWeaponIcon(nullptr);
		CharacterController->SetHUDInventoryEquippedWeaponSlotIcon(nullptr);
		Character->SetWeaponType(EWeaponType::EWT_None);
	}
	else if (Weapon == QuickSlot1Weapon)
	{
		QuickSlot1Weapon = nullptr;
		CharacterController->SetHUDWeaponQuickSlot1Icon(nullptr);
		CharacterController->SetHUDInventoryQuickSlot1Icon(nullptr);
	}
	else if (Weapon == QuickSlot2Weapon)
	{
		QuickSlot2Weapon = nullptr;
		CharacterController->SetHUDWeaponQuickSlot2Icon(nullptr);
		CharacterController->SetHUDInventoryQuickSlot2Icon(nullptr);
	}
}

void UCombatComponent::WeaponToQuickSlot1(AWeapon* Weapon)
{
	AWeapon* WeaponToChange = QuickSlot1Weapon;

	QuickSlot1Weapon = Weapon;
	CharacterController->SetHUDWeaponQuickSlot1Icon(Weapon->GetItemIcon());
	CharacterController->SetHUDInventoryQuickSlot1Icon(Weapon->GetItemIcon());

	if (WeaponToChange != nullptr)
	{
		if (Weapon == EquippedWeapon)
		{
			UnEquipWeapon(Weapon);
			WeaponToChange->GetInventorySlot()->EquipButtonClicked();
		}
		else if (Weapon == QuickSlot2Weapon)
		{
			QuickSlot2Weapon = nullptr;
			CharacterController->SetHUDWeaponQuickSlot2Icon(nullptr);
			CharacterController->SetHUDInventoryQuickSlot2Icon(nullptr);

			WeaponToChange->GetInventorySlot()->QuickSlot2ButtonClicked();
		}
	}

	/*if(Weapon == EquippedWeapon)
	{
		UnEquipWeapon(Weapon);
		if (QuickSlot1Weapon != nullptr)
		{
			QuickSlot1Weapon->GetInventorySlot()->EquipButtonClicked();
		}
	}
	else if (Weapon == QuickSlot2Weapon)
	{
		QuickSlot2Weapon = nullptr;
		CharacterController->SetHUDWeaponQuickSlot2Icon(nullptr);
		CharacterController->SetHUDInventoryQuickSlot2Icon(nullptr);
		if (QuickSlot1Weapon != nullptr)
		{
			QuickSlot1Weapon->GetInventorySlot()->QuickSlot2ButtonClicked();
		}
	}
	else if (QuickSlot1Weapon != nullptr)
	{
		QuickSlot1Weapon->GetInventorySlot()->QuickSlot1ButtonClicked();
	}*/
}

void UCombatComponent::WeaponToQuickSlot2(AWeapon* Weapon)
{
	AWeapon* WeaponToChange = QuickSlot2Weapon;

	QuickSlot2Weapon = Weapon;
	CharacterController->SetHUDWeaponQuickSlot2Icon(Weapon->GetItemIcon());
	CharacterController->SetHUDInventoryQuickSlot2Icon(Weapon->GetItemIcon());

	if (WeaponToChange != nullptr)
	{
		if (Weapon == EquippedWeapon)
		{
			UnEquipWeapon(Weapon);
			WeaponToChange->GetInventorySlot()->EquipButtonClicked();
		}
		else if (Weapon == QuickSlot1Weapon)
		{
			QuickSlot1Weapon = nullptr;
			CharacterController->SetHUDWeaponQuickSlot1Icon(nullptr);
			CharacterController->SetHUDInventoryQuickSlot1Icon(nullptr);

			WeaponToChange->GetInventorySlot()->QuickSlot1ButtonClicked();
		}
	}

	/*if (Weapon == EquippedWeapon)
	{
		UnEquipWeapon(Weapon);
		if (QuickSlot2Weapon != nullptr)
		{
			QuickSlot2Weapon->GetInventorySlot()->EquipButtonClicked();
		}
	}
	else if (Weapon == QuickSlot1Weapon)
	{
		QuickSlot1Weapon = nullptr;
		CharacterController->SetHUDWeaponQuickSlot1Icon(nullptr);
		CharacterController->SetHUDInventoryQuickSlot1Icon(nullptr);
		if (QuickSlot2Weapon != nullptr)
		{
			QuickSlot2Weapon->GetInventorySlot()->QuickSlot1ButtonClicked();
		}
	}
	else if(QuickSlot2Weapon != nullptr)
	{
		QuickSlot2Weapon->GetInventorySlot()->QuickSlot2ButtonClicked();
	}

	QuickSlot2Weapon = Weapon;
	CharacterController->SetHUDWeaponQuickSlot2Icon(Weapon->GetItemIcon());
	CharacterController->SetHUDInventoryQuickSlot2Icon(Weapon->GetItemIcon());*/
}

void UCombatComponent::DiscardWeapon(AWeapon* Weapon)
{
	UnEquipWeapon(Weapon);

	FVector DropLoc = Character->GetActorLocation() + FVector(50.f, 0.f, 100.f);
	Weapon->SetActorLocation(DropLoc);

	AcquiredWeapons.Remove(Weapon);
	Weapon->GetInventorySlot()->SetSlottedWeapon(nullptr);
	Weapon->GetInventorySlot()->InitializeIcon();
	HUD->CharacterOverlay->InventoryWidget->SlotArray.Remove(Weapon->GetInventorySlot());
	HUD->CharacterOverlay->InventoryWidget->SlotArray.Add(Weapon->GetInventorySlot());
	HUD->UpdateInventory();
	Weapon->SetInventorySlot(nullptr);
	Weapon->SetOwner(nullptr);
	Weapon->WeaponStateChanged.RemoveAll(this);
}
