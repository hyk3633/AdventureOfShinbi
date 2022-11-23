// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/AmmoType.h"
#include "CombatComponent.generated.h"

class AAOSCharacter;
class UAOSAnimInstance;
class AAOSController;
class UItemComponent;
class AAOSHUD;
class UAnimMontage;
class AItem;
class AWeapon;
class UCurveFloat;
class USoundCue;

DECLARE_MULTICAST_DELEGATE(DPlayerDeathDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOFSHINBI_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	friend AAOSCharacter;

	friend UItemComponent;

	UCombatComponent();

	void MeleeAttack();

	void GunFire();

	void WeaponRightClickSkill();

	void WeaponSkill1();

	void WeaponSkill2();

	void WeaponSkill3();

	DPlayerDeathDelegate PlayerDeathDelegate;

	void HealBan(float HealBanDurationTime);

	void DecreaseDamage(float DmgDecreaDurationTime);

	void CirclingWolvesEnd();

	void UltimateWolfRushEnd();

protected:

	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 신비의 검 몽타주 */

	void PlayMontageOneHandAttack();

	void PlayMontageWolfAttack();

	UFUNCTION()
	void WolfAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	void WolfAttackCoolTimeEnd();

	void PlayMontageCirclingWolves();

	void CirclingWolvesCoolTimeEnd();

	void PlayMontageUltimateWolfRush();

	void UltimateWolfRushCoolTimeEnd();

	/**  */

	void PlayMontageTwoHandAttack();

	/** 총기류 */

	void PlayReloadMontage();

	UFUNCTION()
	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void PlayMontageGunFire();

	void RangedWeaponFire();

	void Zoom(float DeltaTime);

	void Reload();

	/** 글레이브 몽타주 */

	void GlaiveAttack();

	void PlayMontageSickleAttack();
	void PlayMontageGlaiveAttack();
	void PlayMontageGlaiveUltimateAttack(FName Version);

	void MovingCamera(float DeltaTime);

	void CameraMoveEnd();

	bool CheckAbleGlaiveUltiSkill();

	UFUNCTION()
	void GlaiveUltimateAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	void PlayHitReactMontage(FName SectionName);

	UFUNCTION()
	void OnHitReactMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	void PlayMontageDeath();

	UFUNCTION(BlueprintCallable)
	void ActivateWeaponTrace();

	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponTrace();

	UFUNCTION(BlueprintCallable)
	void OnDeathMontageEnded();

	void ResetCombo();

	void DeactivateFireFactor();

	// 플레이어 스탯 처리

	void UpdateHealth(float Damage);

	void UpdateMana(float ManaToSpend);

	void UpdateStamina(float DeltaTime);

	void SetCrosshair();

	void SpreadCrosshair(float DeltaTime);

	void PickingUpItem(AItem* PickedItem);

	void WeaponQuickSwap();

	void OnChangedWeaponState(AWeapon* Weapon);

	void EquipWeapon(AWeapon* Weapon);

	void UnEquipWeapon(AWeapon* Weapon);

	void WeaponToQuickSlot1(AWeapon* Weapon);

	void WeaponToQuickSlot2(AWeapon* Weapon);

	void DiscardWeapon(AWeapon* Weapon);

	void DmgDebuffEnd();

	void HealBanEnd();

private:

	AAOSCharacter* Character;

	UAOSAnimInstance* AnimInstance;

	AAOSController* CharacterController;

	UItemComponent* ItemComp;

	AAOSHUD* HUD;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0", ClampMax = "10000.0"))
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0", ClampMax = "10000.0"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0", ClampMax = "10000.0"))
	float Mana = 100.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0", ClampMax = "10000.0"))
	float MaxMana = 100.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0", ClampMax = "10000.0"))
	float Stamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0", ClampMax = "10000.0"))
	float MaxStamina = 100.f;

	float StaminaDecreaseRate = 20.f;

	float StaminaIncreaseRate = 20.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon = nullptr;

	AWeapon* QuickSlot1Weapon = nullptr;
	AWeapon* QuickSlot2Weapon = nullptr;

	int8 MeleeAttackComboCount = 0;
	int8 RandomMontageNum;

	FTimerHandle ComboTimer;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float ComboTime = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Voice")
	USoundCue* VoiceLowHealth;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Voice")
	USoundCue* VoiceLackMana;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Voice")
	USoundCue* VoiceBreathingRun;

	UAudioComponent* VoiceBreathingRunComp = nullptr;

	bool bVoiceLowHealthPlayed = false;
	bool bVoiceLackManaPlayed = false;

	/** 신비의 검 변수들 */

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Shinbi Sword")
	UAnimMontage* MeleeOneHandAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Shinbi Sword")
	UAnimMontage* WolfAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Shinbi Sword")
	UAnimMontage* CirclingWolvesMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Shinbi Sword")
	UAnimMontage* UltimateWolfRushMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Shinbi Sword")
	UAnimMontage* OneHandHitReactMontage;

	bool bAbleWolfAttack = true;

	FTimerHandle WolfAttackCoolTimer;
	float WolfAttackCoolTime = 1.f;

	bool bAbleCirclingWolves = true;

	FTimerHandle CirclingWolvesCoolTimer;
	float CirclingWolvesCoolTime = 11.f;

	bool bAbleUltimateWolfRush = true;

	FTimerHandle UltimateWolfRushCoolTimer;
	float UltimateWolfRushCoolTime = 20.f;

	/**  */

	UPROPERTY(EditAnywhere)
	UAnimMontage* MeleeTwoHandAttackMontage;

	/** 총기류 */

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Guns")
	UAnimMontage* GunReloadMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Guns")
	UAnimMontage* GunFireMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Guns")
	UAnimMontage* GunHitReactMontage;

	/** 글레이브 */

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Glaive")
	UAnimMontage* SickleAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Glaive")
	UAnimMontage* GlaiveAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Glaive")
	UAnimMontage* GlaiveUltimateMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Glaive")
	UAnimMontage* GlaiveHitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Glaive")
	UCurveFloat* CameraCurve;

	FTimerHandle MovingCameraTimer;

	bool bMovingCamera = false;

	/**  */

	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathMontage;

	bool IsAnimationPlaying = false;

	bool bCanRunning = true;

	float DefaultFOV = 0.f;

	float CurrentFOV = 0.f;

	// 값을 저장해서 쓸건지?
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 50.f;

	UPROPERTY(EditAnywhere)
	float ZoomSpeed = 30.f;

	FTimerHandle FireFactorTimer;
	float FireFactorTime = 0.05f;
	bool bFireFactor = false;

	float CrosshairVelocityFactor = 0.f;
	float CrosshairInAirFactor = 0.f;
	float CrosshairZoomFactor = 0.f;
	float CrosshairFireFactor = 0.f;

	TArray<AWeapon*> AcquiredWeapons;

	FTimerHandle DamageDebuffTimer;
	FTimerHandle HealBanTimer;

	bool bDmgDebuffActivated = false;
	bool bHealBanActivated = false;

	bool bEnableCheck = true;


public:

	AWeapon* GetEquippedWeapon() const;
	float GetHealthPercentage() const;
	bool SpendStamina(float StaminaToSpend);
	bool SpendMana(float ManaToSpend);
	bool GetDmgDebuffActivated() const;
	bool GetHealBanActivated() const;
	bool GetEnableCheck() const;
	void SetEnableCheck(bool bCheck);

};
