

#include "Components/CombatComponent.h"
#include "Components/ItemComponent.h"
#include "Components/TextBlock.h"
#include "Components/AudioComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AOSCharacter.h"
#include "Player/AOSAnimInstance.h"
#include "Player/AOSController.h"
#include "System/AOSGameModeBase.h"
#include "HUD/InventorySlot.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/Item.h"
#include "Items/ItemAmmo.h"
#include "Weapons/Weapon.h"
#include "Weapons/MeleeWeapon.h"
#include "Weapons/ShinbiSword.h"
#include "Weapons/Glaive.h"
#include "Weapons/RangedWeapon.h"
#include "Weapons/RangedHitScanWeapon.h"
#include "Weapons/RangedProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Types/WeaponState.h"
#include "Sound/SoundCue.h"
#include "..\..\Public\Components\CombatComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::RestartCombatComp()
{
	CharacterController = Character->GetController<AAOSController>();
	if (CharacterController)
	{
		CharacterController->SetHUDHealthBar(Health, MaxHealth);
		CharacterController->SetHUDManaBar(Mana, MaxMana);
		CharacterController->SetHUDStaminaBar(Stamina, MaxStamina);
	}

	GameMode = GetWorld()->GetAuthGameMode<AAOSGameModeBase>();
	if (GameMode && GameMode->GetWeaponCount() > 0)
	{
		for (int32 i = 0; i < GameMode->GetWeaponCount(); i++)
		{
			GameMode->GetWeaponItem(i)->SetWeaponState(EWeaponState::EWS_PickedUp);
			GameMode->GetWeaponItem(i)->WeaponStateChanged.AddUObject(this, &UCombatComponent::OnChangedWeaponState);
		}
		if (GameMode->GetEquippedWeapon())
		{
			GameMode->GetEquippedWeapon()->SetWeaponState(EWeaponState::EWS_Equipped);
		}
		if (GameMode->GetQuickSlot1Weapon())
		{
			GameMode->GetQuickSlot1Weapon()->SetWeaponState(EWeaponState::EWS_QuickSlot1);
		}
		if (GameMode->GetQuickSlot2Weapon())
		{
			GameMode->GetQuickSlot2Weapon()->SetWeaponState(EWeaponState::EWS_QuickSlot2);
		}
	}
}

void UCombatComponent::SettingAfterLevelTransition()
{
	CharacterController = Character->GetController<AAOSController>();
	if (CharacterController)
	{
		CharacterController->SetHUDHealthBar(Health, MaxHealth);
		CharacterController->SetHUDManaBar(Mana, MaxMana);
		CharacterController->SetHUDStaminaBar(Stamina, MaxStamina);
	}

	GameMode = GetWorld()->GetAuthGameMode<AAOSGameModeBase>();
	if (GameMode)
	{
		if (GameMode->GetEquippedWeapon())
		{
			GameMode->GetEquippedWeapon()->GetInventorySlot()->EquipButtonClicked();
		}
		if (GameMode->GetQuickSlot1Weapon())
		{
			GameMode->GetQuickSlot1Weapon()->GetInventorySlot()->QuickSlot1ButtonClicked();
		}
		if (GameMode->GetQuickSlot2Weapon())
		{
			GameMode->GetQuickSlot2Weapon()->GetInventorySlot()->QuickSlot2ButtonClicked();
		}
	}
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = Cast<UAOSAnimInstance>(Character->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UCombatComponent::WolfAttackMontageEnd);
		AnimInstance->OnMontageEnded.AddDynamic(this, &UCombatComponent::OnReloadMontageEnded);
	}

	CharacterController = Character->GetController<AAOSController>();
	if (CharacterController)
	{
		CharacterController->SetHUDHealthBar(Health, MaxHealth);
		CharacterController->SetHUDManaBar(Mana, MaxMana);
		CharacterController->SetHUDStaminaBar(Stamina, MaxStamina);
	}

	GameMode = GetWorld()->GetAuthGameMode<AAOSGameModeBase>();

	DefaultFOV = Character->GetCamera()->FieldOfView;
	CurrentFOV = DefaultFOV;
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Zoom(DeltaTime);

	UpdateStamina(DeltaTime);

	SpreadCrosshair(DeltaTime);

	MovingCamera(DeltaTime);
}

void UCombatComponent::MeleeAttack()
{
	if (Character->GetIsAnimationPlaying()) 
		return;

	if (EquippedWeapon)
	{
		Character->GetWorldTimerManager().ClearTimer(ComboTimer);
		Character->GetWorldTimerManager().SetTimer(ComboTimer, this, &UCombatComponent::ResetCombo, ComboTime);

		switch (EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_ShinbiSword:
			PlayMontageOneHandAttack();
			break;
		case EWeaponType::EWT_Glaive:
			GlaiveAttack();
			break;
		}
	}
}

void UCombatComponent::GunFire()
{
	RangedWeaponFire();
}

void UCombatComponent::WeaponRightClickSkill()
{
	if (bAbleWolfAttack)
	{
		PlayMontageWolfAttack();
	}
}

void UCombatComponent::WeaponSkill1()
{
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_ShinbiSword)
	{
		if (bAbleCirclingWolves)
		{
			PlayMontageCirclingWolves();
		}
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Glaive)
	{
		if (CheckAbleGlaiveUltiSkill())
		{
			PlayMontageGlaiveUltimateAttack(FName("U1"));
		}
	}
}

void UCombatComponent::WeaponSkill2()
{
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_ShinbiSword)
	{
		if (bAbleUltimateWolfRush)
		{
			PlayMontageUltimateWolfRush();
		}
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Glaive)
	{
		if (CheckAbleGlaiveUltiSkill())
		{
			PlayMontageGlaiveUltimateAttack(FName("U2"));
		}
	}
	else
	{
		ARangedWeapon* RW = Cast<ARangedWeapon>(EquippedWeapon);
		if (RW)
		{
			if (RW->GetLoadedAmmo() < RW->GetMagazine() && GameMode->GetAmmoQuantity(RW->GetAmmoType()) > 0)
			{
				PlayReloadMontage();
			}
		}
	}
}

void UCombatComponent::HealBan(float HealBanDurationTime)
{
	bHealBanActivated = true;

	if (HealBanSignClass)
	{
		HealBanSign = CreateWidget<UUserWidget>(GetWorld(), HealBanSignClass);
		if (HealBanSign)
		{
			HealBanSign->AddToViewport();
		}
	}

	Character->GetWorldTimerManager().SetTimer(HealBanTimer, this, &UCombatComponent::HealBanEnd, HealBanDurationTime);
}

void UCombatComponent::DecreaseDamage(float DmgDecreaDurationTime)
{
	bDmgDebuffActivated = true;
	if (AttackDebuffSignClass)
	{
		AttackDebuffSign = CreateWidget<UUserWidget>(GetWorld(), AttackDebuffSignClass);
		if (AttackDebuffSign)
		{
			AttackDebuffSign->AddToViewport();
		}
	}

	Character->GetWorldTimerManager().SetTimer(DamageDebuffTimer, this, &UCombatComponent::DmgDebuffEnd, DmgDecreaDurationTime);
}

void UCombatComponent::PlayMontageOneHandAttack()
{
	if (AnimInstance == nullptr || MeleeOneHandAttackMontage == nullptr) 
		return;

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

void UCombatComponent::PlayMontageWolfAttack()
{
	if (AnimInstance == nullptr || WolfAttackMontage == nullptr)
		return;

	AShinbiSword* SS = Cast<AShinbiSword>(EquippedWeapon);
	if (SS == nullptr)
		return;

	if(SpendMana(SS->GetSkillMana(EShinbiSkill::ESS_WolfAttack)) == false)
		return;

	bAbleWolfAttack = false;

	AnimInstance->Montage_Play(WolfAttackMontage);
}

void UCombatComponent::WolfAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == WolfAttackMontage)
	{
		Character->GetWorldTimerManager().SetTimer(WolfAttackCoolTimer, this, &UCombatComponent::WolfAttackCoolTimeEnd, WolfAttackCoolTime);
	}
}

void UCombatComponent::WolfAttackCoolTimeEnd()
{
	bAbleWolfAttack = true;
}

void UCombatComponent::PlayMontageCirclingWolves()
{
	if (AnimInstance == nullptr || CirclingWolvesMontage == nullptr)
		return;

	AShinbiSword* SS = Cast<AShinbiSword>(EquippedWeapon);
	if (SS == nullptr)
		return;

	if (SpendMana(SS->GetSkillMana(EShinbiSkill::ESS_CirclingWolves)) == false)
		return;

	bAbleCirclingWolves = false;

	AnimInstance->Montage_Play(CirclingWolvesMontage);
}

void UCombatComponent::CirclingWolvesEnd()
{
	Character->GetWorldTimerManager().SetTimer(CirclingWolvesCoolTimer, this, &UCombatComponent::CirclingWolvesCoolTimeEnd, CirclingWolvesCoolTime);
}

void UCombatComponent::CirclingWolvesCoolTimeEnd()
{
	bAbleCirclingWolves = true;
}

void UCombatComponent::PlayMontageUltimateWolfRush()
{
	if (AnimInstance == nullptr || UltimateWolfRushMontage == nullptr)
		return;

	AShinbiSword* SS = Cast< AShinbiSword>(EquippedWeapon);
	if (SS == nullptr)
		return;

	if (SpendMana(SS->GetSkillMana(EShinbiSkill::ESS_UltimateWolfAttack)) == false)
		return;

	bAbleUltimateWolfRush = false;

	AnimInstance->Montage_Play(UltimateWolfRushMontage);
}

void UCombatComponent::UltimateWolfRushEnd()
{
	Character->GetWorldTimerManager().SetTimer(UltimateWolfRushCoolTimer, this, &UCombatComponent::UltimateWolfRushCoolTimeEnd, UltimateWolfRushCoolTime);
}

void UCombatComponent::UltimateWolfRushCoolTimeEnd()
{
	bAbleUltimateWolfRush = true;
}

void UCombatComponent::PlayReloadMontage()
{
	if (GunReloadMontage == nullptr)
		return;

	Character->SetGunRecoil(1.f);

	Character->SetCharacterState(ECharacterState::ECS_AnimationPlaying);

	AnimInstance->Montage_Play(GunReloadMontage);
}

void UCombatComponent::OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != GunReloadMontage)
		return;

	Character->SetCharacterState(ECharacterState::ECS_Nothing);

	ARangedWeapon* RW = Cast<ARangedWeapon>(EquippedWeapon);
	if (RW)
	{
		Character->SetGunRecoil(RW->GetGunRecoil());
	}

	Reload();

	// 공격 버튼이 눌려져 있으면 재장전 후 공격 다시 수행
	if (Character->GetAttackButtonPressing())
	{
		Character->CallAttackFunction();
	}
}

void UCombatComponent::PlayMontageGunFire()
{
	if (AnimInstance == nullptr || GunFireMontage == nullptr) 
		return;

	AnimInstance->Montage_Play(GunFireMontage);
	AnimInstance->Montage_JumpToSection(FName("GunFireFast"));
}

void UCombatComponent::RangedWeaponFire()
{
	ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);
	if (RangedWeapon == nullptr)
		return;

	// 장전된 탄약이 있으면
	if (RangedWeapon->GetLoadedAmmo() > 0)
	{
		bFireFactor = true;
		PlayMontageGunFire();
		RangedWeapon->Firing();
		Character->GetWorldTimerManager().SetTimer(FireFactorTimer, this, &UCombatComponent::DeactivateFireFactor, FireFactorTime);
	}
	else if (RangedWeapon->GetLoadedAmmo() == 0 && GameMode->GetAmmoQuantity(RangedWeapon->GetAmmoType()) > 0) // 장전된 탄약이 없고 소지 탄약이 있으면
	{
		if (Character->GetAttackButtonPressing())
		{
			Character->SetAbleAttackFalse();
		}
		PlayReloadMontage();
	}
	else // 장전된 탄약도 소지한 탄약도 없으면 딸깍 소리 출력
	{
		RangedWeapon->PlayNoAmmoSound();
	}

	if (CharacterController)
	{
		CharacterController->SetHUDLoadedAmmoText(RangedWeapon->GetLoadedAmmo());
	}
}

void UCombatComponent::Zoom(float DeltaTime)
{
	if (Character == nullptr || EquippedWeapon == nullptr) 
		return;

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
	if (ItemComp == nullptr) 
		return;

	ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);

	const EAmmoType AmmoType = RangedWeapon->GetAmmoType();
	const int32 TotalAmmo = GameMode->GetAmmoQuantity(AmmoType);
	if (TotalAmmo == 0) 
		return;

	int32 AmmoToReload = RangedWeapon->GetMagazine() - RangedWeapon->GetLoadedAmmo();
	// 재장전할 탄약이 소지한 탄약보다 적으면 재장전할 탄약 만큼만 재장전
	if (AmmoToReload <= TotalAmmo)
	{
		RangedWeapon->SetLoadedAmmo(RangedWeapon->GetMagazine());
		GameMode->AddAmmoQuantity(AmmoType, -AmmoToReload);
	}
	else // 재장전할 탄약이 소지한 탄약보다 많으면 소지한 탄약 전부 장전
	{
		RangedWeapon->SetLoadedAmmo(RangedWeapon->GetLoadedAmmo() + TotalAmmo);
		GameMode->AddAmmoQuantity(AmmoType, -TotalAmmo);
	}

	if (CharacterController)
	{
		CharacterController->SetHUDLoadedAmmoText(RangedWeapon->GetLoadedAmmo());
		CharacterController->SetHUDTotalAmmoText(GameMode->GetAmmoQuantity(AmmoType));
		CharacterController->UpdateAmmo(AmmoType);
	}
}

void UCombatComponent::GlaiveAttack()
{
	if (AnimInstance == nullptr) 
		return;

	AGlaive* Glaive = Cast<AGlaive>(EquippedWeapon);
	if (Glaive == nullptr)
		return;

	if (Glaive->GetSickleMode())
	{
		PlayMontageSickleAttack();
	}
	else
	{
		PlayMontageGlaiveAttack();
	}
}

void UCombatComponent::PlayMontageSickleAttack()
{
	if (SickleAttackMontage == nullptr)
		return;

	AnimInstance->Montage_Play(SickleAttackMontage);
	if (Character->GetCharacterMovement()->IsFalling())
	{
		AnimInstance->Montage_JumpToSection(FName("Air"));
	}
	else
	{
		if (MeleeAttackComboCount == 0)
		{
			AnimInstance->Montage_JumpToSection(FName("A"));
			MeleeAttackComboCount++;
		}
		else if (MeleeAttackComboCount == 1)
		{
			AnimInstance->Montage_JumpToSection(FName("B"));
			MeleeAttackComboCount++;
		}
		else if (MeleeAttackComboCount == 2)
		{
			AnimInstance->Montage_JumpToSection(FName("C"));
			MeleeAttackComboCount++;
		}
		else
		{
			AnimInstance->Montage_JumpToSection(FName("D"));
			ResetCombo();
		}
	}
}

void UCombatComponent::PlayMontageGlaiveAttack()
{
	if (GlaiveAttackMontage == nullptr)
		return;

	AnimInstance->Montage_Play(GlaiveAttackMontage);
	if (Character->GetCharacterMovement()->IsFalling())
	{
		return;
	}
	else
	{
		if (MeleeAttackComboCount == 0)
		{
			AnimInstance->Montage_JumpToSection(FName("A"));
			MeleeAttackComboCount++;
		}
		else if (MeleeAttackComboCount == 1)
		{
			AnimInstance->Montage_JumpToSection(FName("C"));
			MeleeAttackComboCount++;
		}
		else
		{
			AnimInstance->Montage_JumpToSection(FName("B"));
			ResetCombo();
		}
	}
}

void UCombatComponent::PlayMontageGlaiveUltimateAttack(FName Version)
{
	if (AnimInstance == nullptr || GlaiveUltimateMontage == nullptr)
		return;

	AGlaive* Glaive = Cast<AGlaive>(EquippedWeapon);
	if (Glaive == nullptr)
		return;

	if (SpendMana(Glaive->GetUltiSkillMana()) == false)
		return;

	AnimInstance->Montage_Play(GlaiveUltimateMontage);
	AnimInstance->Montage_JumpToSection(Version);

	// 카메라를 캐릭터에게서 분리
	Character->GetCamera()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Character->GetWorldTimerManager().SetTimer(MovingCameraTimer, this, &UCombatComponent::CameraMoveEnd, 0.8f);
	bMovingCamera = true;
}

void UCombatComponent::MovingCamera(float DeltaTime)
{
	if (bMovingCamera == false || CameraCurve == nullptr)
		return;

	float Sign = 1.f;

	// 타이머 경과 시간
	const float ElapsedTime = Character->GetWorldTimerManager().GetTimerElapsed(MovingCameraTimer);
	
	// 경과 시간이 0.6 이상이면 
	if (ElapsedTime >= 0.6f)
		Sign *= -1.f;

	const float CurveValue = CameraCurve->GetFloatValue(ElapsedTime);

	FVector CameraLocaion = Character->GetCamera()->GetComponentLocation();
	CameraLocaion.Z += CurveValue * Sign;

	Character->GetCamera()->SetWorldLocation(CameraLocaion);
}

void UCombatComponent::CameraMoveEnd()
{
	bMovingCamera = false;
	Character->GetCamera()->AttachToComponent(Character->GetSpringArm(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Character->GetCamera()->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
}

bool UCombatComponent::CheckAbleGlaiveUltiSkill()
{
	AGlaive* Glaive = Cast<AGlaive>(EquippedWeapon);
	if (Glaive == nullptr)
		return false;

	if (Glaive->GetHitStackIsFull() && Glaive->GetUltiSkillMana() <= Mana)
		return true;
	else
		return false;
}

void UCombatComponent::ResetCombo()
{
	MeleeAttackComboCount = 0;
}

void UCombatComponent::DeactivateFireFactor()
{
	bFireFactor = false;
}

void UCombatComponent::PlayHitReactMontage(FName SectionName)
{
	if (AnimInstance == nullptr || EquippedWeapon == nullptr)
		return;

	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_ShinbiSword)
	{
		if (OneHandHitReactMontage)
		{
			AnimInstance->Montage_Play(OneHandHitReactMontage);
		}
	}
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Glaive)
	{
		if (GlaiveHitReactMontage)
		{
			AnimInstance->Montage_Play(GlaiveHitReactMontage);
		}
	}
	else
	{
		if (GunHitReactMontage)
		{
			AnimInstance->Montage_Play(GunHitReactMontage);
		}
	}
	AnimInstance->Montage_JumpToSection(SectionName);
}

void UCombatComponent::PlayMontageDeath()
{
	if (AnimInstance == nullptr || DeathMontage == nullptr) 
		return;
	
	AnimInstance->StopAllMontages(0.f);
	AnimInstance->Montage_Play(DeathMontage);
}

void UCombatComponent::ActivateWeaponTrace()
{
	AMeleeWeapon* MW = Cast<AMeleeWeapon>(EquippedWeapon);
	if (MW)
	{
		MW->ActivateWeaponTrace();
	}
}

void UCombatComponent::DeactivateWeaponTrace()
{
	AMeleeWeapon* MW = Cast<AMeleeWeapon>(EquippedWeapon);
	if (MW)
	{
		MW->DeactivateWeaponTrace();
	}
}

void UCombatComponent::OnDeathMontageEnded()
{
	Character->GetMesh()->bPauseAnims = true;
}

void UCombatComponent::UpdateHealth(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	if (CharacterController)
	{
		CharacterController->SetHUDHealthBar(Health, MaxHealth);
	}

	if (Health == 0.f)
	{
		PlayerDeathDelegate.Broadcast();
		PlayMontageDeath();
		RemoveSign();
		if (EquippedWeapon)
		{
			EquippedWeapon->DropWeapon();
		}
	}
	else if (GetHealthPercentage() < 0.3f && bVoiceLowHealthPlayed == false) // 체력이 30% 이하면 목소리 출력
	{
		if (VoiceLowHealth)
		{
			UGameplayStatics::PlaySound2D(this, VoiceLowHealth);
		}
		bVoiceLowHealthPlayed = true;
	}
}

void UCombatComponent::UpdateMana(float ManaToSpend)
{
	Mana = FMath::Clamp(Mana - ManaToSpend, 0.f, MaxMana);
	if (CharacterController)
	{
		CharacterController->SetHUDManaBar(Mana, MaxMana);
	}
}

void UCombatComponent::UpdateStamina(float DeltaTime)
{
	// 지구력이 0 이상이고 달릴 수 있는 상태면
	if (FMath::FloorToFloat(Stamina) > 0.f && bCanRunning)
	{
		// 달리고 있으면 지구력 감소 아니면 증가
		if (Character->GetIsRunning())
		{
			const float Decrease = DeltaTime * StaminaDecreaseRate;
			Stamina = FMath::Clamp(Stamina - Decrease, 0.f, MaxStamina);
		}
		else
		{
			if (Stamina < MaxStamina)
			{
				const float Increase = DeltaTime * StaminaIncreaseRate;
				Stamina = FMath::Clamp(Stamina + Increase, 0.f, MaxStamina);
			}
		}
	}
	else
	{
		// 지구력을 0 까지 소모했으면 달리기 불가 상태로 설정 
		if (FMath::FloorToFloat(Stamina) == 0.f)
		{
			bCanRunning = false;
			Character->StopRunning();
		}

		// 지구력이 60% 이하면 지구력 계속 증가
		if (Stamina <= MaxStamina * 0.6f)
		{
			const float Increase = DeltaTime * StaminaIncreaseRate;
			Stamina = FMath::Clamp(Stamina + Increase, 0.f, MaxStamina);
		}
		else // 지구력이 60% 초과되면 달리기 가능 상태로 설정
		{
			bCanRunning = true;
			if (Character->GetIsRunning()) Character->StartRunning();
		}
	}

	// 지구력이 40% 이하면 목소리 출력
	if (Stamina <= MaxStamina * 0.4f)
	{
		if (VoiceBreathingRunComp == nullptr)
		{
			if (VoiceBreathingRun)
			{
				VoiceBreathingRunComp = UGameplayStatics::SpawnSound2D(this, VoiceBreathingRun);
			}
		}
		else
		{
			if (VoiceBreathingRunComp->IsPlaying() == false)
			{
				VoiceBreathingRunComp->IsPlaying();
			}
		}
	}
	else
	{
		if (VoiceBreathingRunComp)
		{
			if (VoiceBreathingRunComp->IsPlaying() == false)
			{
				VoiceBreathingRunComp->Deactivate();
				VoiceBreathingRunComp->DestroyComponent();
				VoiceBreathingRunComp = nullptr;
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
	if (CharacterController == nullptr) 
		return;

	if (EquippedWeapon)
	{
		ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);
		if (RangedWeapon)
		{
			CharacterController->SetHUDCrosshairs(RangedWeapon->GetCrosshairs());
		}
		else
		{
			CharacterController->EraseHUDCrosshairs();
		}
	}
	else
	{
		CharacterController->EraseHUDCrosshairs();
	}
}

void UCombatComponent::SpreadCrosshair(float DeltaTime)
{
	if (CharacterController == nullptr)
		return;

	FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
	FVector2D VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// 점프, 정조준, 발사에 따라 크로스헤어가 퍼지도록 설정
	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	if (Character->GetIsAiming())
	{
		CrosshairZoomFactor = FMath::FInterpTo(CrosshairInAirFactor, 1.f, DeltaTime, 10.f);
	}
	else
	{
		CrosshairZoomFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 5.f);
	}

	if (bFireFactor)
	{
		CrosshairFireFactor = FMath::FInterpTo(CrosshairFireFactor, 1.5f, DeltaTime, 10.f);
	}
	else
	{
		CrosshairFireFactor = FMath::FInterpTo(CrosshairFireFactor, 0.f, DeltaTime, 3.f);
	}

	CharacterController->SetCrosshairSpread(CrosshairVelocityFactor + CrosshairInAirFactor + CrosshairFireFactor - CrosshairZoomFactor);
}

void UCombatComponent::PickingUpItem(AItem* PickedItem)
{
	if (CharacterController == nullptr || PickedItem == nullptr)
		return;

	// 아이템의 움직임 비활성화
	PickedItem->DeactivateItemMovement();

	// 아이템이 무기라면
	AWeapon* PickedWeapon = Cast<AWeapon>(PickedItem);
	if (PickedWeapon)
	{
		// 인벤토리 슬롯이 꽉 찼다면 새로 생성
		if (GameMode->GetWeaponCount() == CharacterController->GetHUDInventorySlotCount())
		{
			CharacterController->CreateHUDInventorySlot();
		}
		GameMode->AddWeaponToArr(PickedWeapon);
		PickedWeapon->SetWeaponState(EWeaponState::EWS_PickedUp);
		PickedWeapon->WeaponStateChanged.AddUObject(this, &UCombatComponent::OnChangedWeaponState);
		CharacterController->AddWeaponToSlot(GameMode->GetWeaponCount() - 1, PickedWeapon);
	}
	else
	{
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
	// 장착되어 있던 무기
	AWeapon* WeaponToChange = EquippedWeapon;

	// 게임 모드에 무기 저장
	GameMode->KeepEquippedWeapon(Weapon);
	EquippedWeapon = Weapon; // 새로 장착한 무기

	SetCrosshair();

	FName SocketName;
	ARangedWeapon* RW = Cast<ARangedWeapon>(EquippedWeapon);
	if (RW)
	{
		// 원거리 무기의 데미지 계산 설정
		DefaultValue = 2.f;
		RandRangeValue = 5;

		AItemAmmo* Ammo = Cast<AItemAmmo>(RW->GetAmmoItem());
		if (Ammo)
		{
			// 탄약이 없으면 기본 탄약 추가
			if (GameMode->IsAmmoTypeExist(Ammo->GetAmmoType()) == false)
			{
				Character->GetItemComp()->AddItem(RW->GetAmmoItem());
			}
		}

		// 탄약 HUD 활성화
		CharacterController->SetHUDLoadedAmmoText(RW->GetLoadedAmmo());
		CharacterController->SetHUDTotalAmmoText(GameMode->GetAmmoQuantity(RW->GetAmmoType()));
		CharacterController->HUDAmmoInfoOn();

		// 장전
		if (RW->GetLoadedAmmo() == 0 && GameMode->GetAmmoQuantity(RW->GetAmmoType()) > 0)
		{
			Reload();
		}

		// 무기에 따라 다른 소켓 위치에 장착
		Character->SetGunRecoil(RW->GetGunRecoil());
		if (RW->GetWeaponType() == EWeaponType::EWT_Revenent)
		{
			SocketName = FName("RevenentSocket");
		}
		else if (RW->GetWeaponType() == EWeaponType::EWT_Wraith)
		{
			SocketName = FName("WraithSocket");
		}
		else if (RW->GetWeaponType() == EWeaponType::EWT_AK47)
		{
			SocketName = FName("AK47Socket");
		}
		else
		{
			SocketName = FName("GunSocket");
		}
	}
	else
	{
		// 근접 무기의 데미지 계산 설정
		DefaultValue = 5.f;
		RandRangeValue = 15;

		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_ShinbiSword)
		{
			SocketName = FName("OneHandSocket");
		}
		else if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Glaive)
		{
			SocketName = FName("GlaveSocket");
		}

		CharacterController->HUDAmmoInfoOff();
	}

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	// 장착 슬롯 아이콘 설정
	CharacterController->SetHUDEquippedWeaponIcon(EquippedWeapon->GetItemIcon());
	CharacterController->SetHUDInventoryEquippedWeaponSlotIcon(EquippedWeapon->GetItemIcon());

	EquippedWeapon->SetOwner(Character);
	AnimInstance->SetWeaponType(EquippedWeapon->GetWeaponType());

	// 장착되어 있던 무기가 nullptr 이 아니면
	if (WeaponToChange != nullptr)
	{
		// 새로 장착한 무기가 퀵슬롯1에 장착했던 무기라면
		if (Weapon == QuickSlot1Weapon)
		{
			QuickSlot1Weapon = nullptr;
			WeaponToChange->GetInventorySlot()->QuickSlot1ButtonClicked();
		}
		else if (Weapon == QuickSlot2Weapon) // 새로 장착한 무기가 퀵슬롯2에 장착했던 무기라면
		{
			QuickSlot2Weapon = nullptr;
			WeaponToChange->GetInventorySlot()->QuickSlot2ButtonClicked();
		}
		else
		{
			WeaponToChange->GetInventorySlot()->EquipButtonClicked();
		}
	}
	else // 장착되어 있던 무기가 nullptr 이라면 (장착되어 있던 무기가 없었을 경우)
	{
		if (Weapon == QuickSlot1Weapon)
		{
			QuickSlot1Weapon = nullptr;
			GameMode->KeepQuickSlot1Weapon(nullptr);
			CharacterController->SetHUDWeaponQuickSlot1Icon(nullptr);
			CharacterController->SetHUDInventoryQuickSlot1Icon(nullptr);
		}
		else if (Weapon == QuickSlot2Weapon)
		{
			QuickSlot2Weapon = nullptr;
			GameMode->KeepQuickSlot2Weapon(nullptr);
			CharacterController->SetHUDWeaponQuickSlot2Icon(nullptr);
			CharacterController->SetHUDInventoryQuickSlot2Icon(nullptr);
		}
	}

	// 이동 속도, 시야, 컨트롤 설정
	Character->SetWalkingSpeed(EWalkingState::EWS_Armed);
	Character->SetView(Weapon->GetWeaponType());
	Character->ActivateWeaponControlMode();
}

void UCombatComponent::UnEquipWeapon(AWeapon* Weapon)
{
	// 장착한 무기를 장착해제 한 경우
	if (Weapon == EquippedWeapon)
	{
		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachRules);
		AnimInstance->SetWeaponType(EWeaponType::EWT_None);
		EquippedWeapon->SetOwner(nullptr);
		EquippedWeapon = nullptr;

		GameMode->KeepEquippedWeapon(nullptr);
		CharacterController->SetHUDEquippedWeaponIcon(nullptr);
		CharacterController->SetHUDInventoryEquippedWeaponSlotIcon(nullptr);
		CharacterController->HUDAmmoInfoOff();

		Character->SetWalkingSpeed(EWalkingState::EWS_UnArmed);
		Character->SetView(EWeaponType::EWT_None);
		Character->DeactivateWeaponControlMode();

		SetCrosshair();
	}
	else if (Weapon == QuickSlot1Weapon)
	{
		QuickSlot1Weapon = nullptr;
		GameMode->KeepQuickSlot1Weapon(nullptr);
		CharacterController->SetHUDWeaponQuickSlot1Icon(nullptr);
		CharacterController->SetHUDInventoryQuickSlot1Icon(nullptr);
	}
	else if (Weapon == QuickSlot2Weapon)
	{
		QuickSlot2Weapon = nullptr;
		GameMode->KeepQuickSlot2Weapon(nullptr);
		CharacterController->SetHUDWeaponQuickSlot2Icon(nullptr);
		CharacterController->SetHUDInventoryQuickSlot2Icon(nullptr);
	}
}

// 무기를 퀵슬롯1로 장착
void UCombatComponent::WeaponToQuickSlot1(AWeapon* Weapon)
{
	AWeapon* WeaponToChange = QuickSlot1Weapon;

	QuickSlot1Weapon = Weapon;
	GameMode->KeepQuickSlot1Weapon(Weapon);
	CharacterController->SetHUDWeaponQuickSlot1Icon(Weapon->GetItemIcon());
	CharacterController->SetHUDInventoryQuickSlot1Icon(Weapon->GetItemIcon());

	// 퀵슬롯1에 무기가 장착되어 있던 경우 
	if (WeaponToChange != nullptr)
	{
		// 퀵슬롯1에 장착할 무기가 장착 슬롯에 장착 되어 있던 무기라면
		if (Weapon == EquippedWeapon)
		{
			UnEquipWeapon(Weapon);
			WeaponToChange->GetInventorySlot()->EquipButtonClicked();
		}
		else if (Weapon == QuickSlot2Weapon) // 퀵슬롯1에 장착할 무기가 퀵슬롯2에 장착 되어 있던 무기라면
		{
			QuickSlot2Weapon = nullptr;
			CharacterController->SetHUDWeaponQuickSlot2Icon(nullptr);
			CharacterController->SetHUDInventoryQuickSlot2Icon(nullptr);
			WeaponToChange->GetInventorySlot()->QuickSlot2ButtonClicked();
		}
	}
}

// 무기를 퀵슬롯2로 장착
void UCombatComponent::WeaponToQuickSlot2(AWeapon* Weapon)
{
	AWeapon* WeaponToChange = QuickSlot2Weapon;

	QuickSlot2Weapon = Weapon;
	GameMode->KeepQuickSlot2Weapon(Weapon);
	CharacterController->SetHUDWeaponQuickSlot2Icon(Weapon->GetItemIcon());
	CharacterController->SetHUDInventoryQuickSlot2Icon(Weapon->GetItemIcon());

	// 퀵슬롯2에 무기가 장착되어 있던 경우 
	if (WeaponToChange != nullptr)
	{
		// 퀵슬롯2에 장착할 무기가 장착 슬롯에 장착 되어 있던 무기라면
		if (Weapon == EquippedWeapon)
		{
			UnEquipWeapon(Weapon);
			WeaponToChange->GetInventorySlot()->EquipButtonClicked();
		}
		else if (Weapon == QuickSlot1Weapon) // 퀵슬롯2에 장착할 무기가 퀵슬롯1에 장착 되어 있던 무기라면
		{
			QuickSlot1Weapon = nullptr;
			CharacterController->SetHUDWeaponQuickSlot1Icon(nullptr);
			CharacterController->SetHUDInventoryQuickSlot1Icon(nullptr);
			WeaponToChange->GetInventorySlot()->QuickSlot1ButtonClicked();
		}
	}
}

// 무기 버림
void UCombatComponent::DiscardWeapon(AWeapon* Weapon)
{
	UnEquipWeapon(Weapon);

	// 떨어트릴 위치
	FVector DropLoc = Character->GetActorLocation() + FVector(50.f, 0.f, 100.f);
	Weapon->SetActorLocation(DropLoc, false, nullptr, ETeleportType::TeleportPhysics);

	GameMode->RemoveWeaponFromArr(Weapon);
	Weapon->GetInventorySlot()->SetSlottedWeapon(nullptr);
	Weapon->GetInventorySlot()->InitializeIcon();
	CharacterController->UpdateInventory(Weapon->GetInventorySlot());
	Weapon->SetInventorySlot(nullptr);
	Weapon->SetOwner(nullptr);
	Weapon->WeaponStateChanged.RemoveAll(this);
}

void UCombatComponent::DmgDebuffEnd()
{
	if (bDmgDebuffActivated)
	{
		bDmgDebuffActivated = false;
		if (AttackDebuffSign)
		{
			AttackDebuffSign->Destruct();
			AttackDebuffSign = nullptr;
		}
	}
}

void UCombatComponent::HealBanEnd()
{
	if (bHealBanActivated)
	{
		bHealBanActivated = false;
		if (HealBanSign)
		{
			HealBanSign->Destruct();
			HealBanSign = nullptr;
		}
	}
}

void UCombatComponent::SetController(AAOSController* Controller)
{
	CharacterController = Controller;
}

void UCombatComponent::SetGameMode(AAOSGameModeBase* Gamemode)
{
	GameMode = Gamemode;
}

void UCombatComponent::SetAnimInstance(UAOSAnimInstance* AnimInst)
{
	AnimInstance = AnimInst;
}

AWeapon* UCombatComponent::GetEquippedWeapon() const
{
	return EquippedWeapon;
}

float UCombatComponent::GetHealthPercentage() const
{
	return Health / MaxHealth;
}

bool UCombatComponent::SpendStamina(float StaminaToSpend)
{
	if (Stamina < StaminaToSpend) 
		return false;

	Stamina = FMath::Clamp(Stamina - StaminaToSpend, 0.f, MaxStamina);
	return true;
}

bool UCombatComponent::SpendMana(float ManaToSpend)
{
	if (Mana < ManaToSpend)
	{
		if (VoiceLackMana && bVoiceLackManaPlayed == false)
		{
			UGameplayStatics::PlaySound2D(this, VoiceLackMana);
			bVoiceLackManaPlayed = true;
		}
		return false;
	}

	UpdateMana(ManaToSpend);
	return true;
}

bool UCombatComponent::GetDmgDebuffActivated() const
{
	return bDmgDebuffActivated;
}

bool UCombatComponent::GetHealBanActivated() const
{
	return bHealBanActivated;
}

float UCombatComponent::GetHealth() const
{
	return Health;
}

float UCombatComponent::GetMana() const
{
	return Mana;
}

void UCombatComponent::SetHealth(float PreHealth)
{
	Health = PreHealth;
}

void UCombatComponent::SetMana(float PreMana)
{
	Mana = PreMana;
}

float UCombatComponent::GetDefaultValue() const
{
	return DefaultValue;
}

int32 UCombatComponent::GetRandRangeValue() const
{
	return RandRangeValue;
}

void UCombatComponent::RemoveSign()
{
	bDmgDebuffActivated = false;
	bHealBanActivated = false;

	if (HealBanSign)
	{
		HealBanSign->Destruct();
		HealBanSign = nullptr;
	}
	if (AttackDebuffSign)
	{
		AttackDebuffSign->Destruct();
		AttackDebuffSign = nullptr;
	}
}
