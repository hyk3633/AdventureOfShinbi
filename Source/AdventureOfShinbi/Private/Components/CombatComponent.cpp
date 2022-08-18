

#include "Components/CombatComponent.h"
#include "Weapons/Weapon.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapons/RangedWeapon.h"
#include "Weapons/RangedHitScanWeapon.h"
#include "Weapons/RangedProjectileWeapon.h"
#include "Components/TextBlock.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeAmmoMap();

	CharacterController = Cast<AAOSController>(Character->GetController());
	if (CharacterController)
	{
		CharacterController->SetHUDAmmoInfoVisibility(false);
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
}

void UCombatComponent::Attack()
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
		case EWeaponType::EWT_Gun:
			PlayMontageGunFire();
			RangedWeaponFire();
			break;
		case EWeaponType::EWT_Glave:
			PlayMontageGlaveAttack();
			break;
		}
	}
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

	if (RangedWeapon->GetLoadedAmmo() == 0 && AmmoMap[RangedWeapon->GetAmmoType()] > 0)
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
	if (Character == nullptr) return;

	if (Character->GetIsAiming())
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, ZoomedFOV, DeltaTime, ZoomSpeed);
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

void UCombatComponent::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_AR, 0);
	AmmoMap.Add(EAmmoType::EAT_SMG, 20);
	AmmoMap.Add(EAmmoType::EAT_Pistol, 0);
	AmmoMap.Add(EAmmoType::EAT_Shell, 0);
	AmmoMap.Add(EAmmoType::EAT_GrenadeLauncher, 0);
	AmmoMap.Add(EAmmoType::EAT_Rocket, 0);
}

void UCombatComponent::Reload()
{
	ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);

	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Gun)
	{
		if (AmmoMap[RangedWeapon->GetAmmoType()] == 0) return;

		int32 AmmoToReload = RangedWeapon->GetMagazine() - RangedWeapon->GetLoadedAmmo();
		if (AmmoToReload < AmmoMap[RangedWeapon->GetAmmoType()])
		{
			RangedWeapon->SetLoadedAmmo(AmmoToReload);
			AmmoMap[RangedWeapon->GetAmmoType()] -= AmmoToReload;
		}
		else
		{
			RangedWeapon->SetLoadedAmmo(AmmoMap[RangedWeapon->GetAmmoType()]);
			AmmoMap[RangedWeapon->GetAmmoType()] = 0;
		}
	}

	if (CharacterController)
	{
		CharacterController->SetHUDLoadedAmmoText(RangedWeapon->GetLoadedAmmo());
		CharacterController->SetHUDTotalAmmoText(AmmoMap[RangedWeapon->GetAmmoType()]);
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
			Character->ResumeRunning();
		}
	}

	if (CharacterController)
	{
		CharacterController->SetHUDStaminaBar(Stamina, MaxStamina);
	}
}

void UCombatComponent::SetEquippedWeapon(AWeapon* Weapon)
{
	EquippedWeapon = Weapon;
	EquippedWeapon->GetWeaponMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// TODO : 캐스팅 연산 옮기기
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Gun)
	{
		ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);
		CharacterController->SetHUDLoadedAmmoText(RangedWeapon->GetLoadedAmmo());
		CharacterController->SetHUDTotalAmmoText(AmmoMap[RangedWeapon->GetAmmoType()]);
		CharacterController->SetHUDAmmoInfoVisibility(true);

		if (RangedWeapon->GetLoadedAmmo() == 0 && AmmoMap[RangedWeapon->GetAmmoType()] > 0)
		{
			Reload();
		}
	}
}