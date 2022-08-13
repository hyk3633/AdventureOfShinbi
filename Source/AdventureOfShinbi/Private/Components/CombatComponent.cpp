

#include "Components/CombatComponent.h"
#include "Weapons/Weapon.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerCharacter.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = true;


}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = Character->GetCamera()->FieldOfView;
	CurrentFOV = DefaultFOV;
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Zoom(DeltaTime);
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
			MeleeOneHandAttack();
			break;
		case EWeaponType::EWT_MeleeTwoHand:
			MeleeTwoHandAttack();
			break;
		case EWeaponType::EWT_Gun:
			GunFire();
			break;
		case EWeaponType::EWT_Glave:
			GlaveAttack();
			break;
		}
	}
}

void UCombatComponent::MeleeOneHandAttack()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || MeleeOneHandAttackMontage == nullptr) return;

	AnimInstance->Montage_Play(MeleeOneHandAttackMontage);
	if (Character->GetCharacterMovement()->IsFalling())
	{
		AnimInstance->Montage_JumpToSection(FName("MeleeOneHandAir"));
	}
	else
	{
		if (MeleeAttackComboCount == 0)
		{
			RandomMontageNum = UKismetMathLibrary::RandomInteger(2);
			if (RandomMontageNum == 0)
			{
				AnimInstance->Montage_JumpToSection(FName("MeleeOneHandC2"));
			}
			else
			{
				AnimInstance->Montage_JumpToSection(FName("MeleeOneHandD"));
			}
			MeleeAttackComboCount++;
		}
		else if (MeleeAttackComboCount == 1)
		{
			if (RandomMontageNum == 0)
			{
				AnimInstance->Montage_JumpToSection(FName("MeleeOneHandD"));
			}
			else
			{
				AnimInstance->Montage_JumpToSection(FName("MeleeOneHandC2"));
			}
			MeleeAttackComboCount++;
		}
		else if (MeleeAttackComboCount == 2)
		{
			if (RandomMontageNum == 0)
			{
				AnimInstance->Montage_JumpToSection(FName("MeleeOneHandC"));
			}
			else
			{
				AnimInstance->Montage_JumpToSection(FName("MeleeOneHandB"));
			}
			MeleeAttackComboCount++;
		}
		else if (MeleeAttackComboCount == 3)
		{
			if (RandomMontageNum == 0)
			{
				AnimInstance->Montage_JumpToSection(FName("MeleeOneHandB"));
			}
			else
			{
				AnimInstance->Montage_JumpToSection(FName("MeleeOneHandC"));
			}
			ResetCombo();
		}
	}

}

void UCombatComponent::MeleeTwoHandAttack()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || MeleeTwoHandAttackMontage == nullptr) return;

	AnimInstance->Montage_Play(MeleeTwoHandAttackMontage);
	if (Character->GetCharacterMovement()->IsFalling())
	{
		//AnimInstance->Montage_JumpToSection(FName("MeleeOneHandAir"));
	}
}

void UCombatComponent::GunFire()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || GunFireMontage == nullptr) return;

	AnimInstance->Montage_Play(GunFireMontage);
	AnimInstance->Montage_JumpToSection(FName("GunFireFast"));
}

void UCombatComponent::GlaveAttack()
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

void UCombatComponent::SetEquippedWeapon(AWeapon* Weapon)
{
	EquippedWeapon = Weapon;
	EquippedWeapon->GetWeaponMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}