
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/AmmoType.h"
#include "CombatComponent.generated.h"

class AAOSCharacter;
class UAOSAnimInstance;
class AAOSGameModeBase;
class AAOSController;
class UItemComponent;
class UAnimMontage;
class AItem;
class AWeapon;
class UCurveFloat;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeathDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOFSHINBI_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	friend AAOSCharacter;

	friend UItemComponent;

	UCombatComponent();

	/** 플레이어 리스폰 후 컴포넌트를 이전 상태로 초기화 */
	void RestartCombatComp();

	/** 레벨 트랜지션 후 컴포넌트를 이전 상태로 초기화 */
	void SettingAfterLevelTransition();

	/** 아이템 획득 처리 */
	void PickingUpItem(AItem* PickedItem);

	/** 공격 함수 */
	void MeleeAttack();

	void GunFire();

	void WeaponRightClickSkill();

	void WeaponSkill1();

	void WeaponSkill2();

	/** 회복 차단 */
	void HealBan(float HealBanDurationTime);

	/** 데미지 감소 */
	void DecreaseDamage(float DmgDecreaDurationTime);

	void CirclingWolvesEnd();

	void UltimateWolfRushEnd();

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDeathDelegate PlayerDeathDelegate;

protected:

	virtual void BeginPlay() override;

	void SetAnimInstance();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** ShinbiSword 관련 함수 */

	void PlayMontageOneHandAttack();

	void PlayMontageWolfAttack();

	UFUNCTION()
	void WolfAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	void WolfAttackCoolTimeEnd();

	void PlayMontageCirclingWolves();

	void CirclingWolvesCoolTimeEnd();

	void PlayMontageUltimateWolfRush();

	void UltimateWolfRushCoolTimeEnd();

	/** 원거리 무기 관련 함수 */

	void PlayReloadMontage();

	UFUNCTION()
	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void PlayMontageGunFire();

	void RangedWeaponFire();

	void Zoom(float DeltaTime);

	void Reload();

	/** Glaive 관련 함수 */

	void GlaiveAttack();

	void PlayMontageSickleAttack();
	void PlayMontageGlaiveAttack();
	void PlayMontageGlaiveUltimateAttack(FName Version);

	void MovingCamera(float DeltaTime);

	void CameraMoveEnd();

	bool CheckAbleGlaiveUltiSkill();

	/** 히트 리액션 몽타주 재생 */
	void PlayHitReactMontage(FName SectionName);

	void PlayMontageDeath();

	UFUNCTION(BlueprintCallable)
	void ActivateWeaponTrace();

	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponTrace();

	UFUNCTION(BlueprintCallable)
	void OnDeathMontageEnded();

	void ResetCombo();

	void DeactivateFireFactor();

	/** 플레이어 스탯 갱신 */

	void UpdateHealth(float Damage);

	void UpdateMana(float ManaToSpend);

	void UpdateStamina(float DeltaTime);

	/** 크로스헤어 설정 */
	void SetCrosshair();

	void SpreadCrosshair(float DeltaTime);

	/** 무기 스왑 */
	void WeaponQuickSwap();

	/** 무기 상태에 따른 처리 함수 */
	void OnChangedWeaponState(AWeapon* Weapon);

	void EquipWeapon(AWeapon* Weapon);

	void UnEquipWeapon(AWeapon* Weapon);

	void WeaponToQuickSlot1(AWeapon* Weapon);

	void WeaponToQuickSlot2(AWeapon* Weapon);

	void DiscardWeapon(AWeapon* Weapon);

	void DmgDebuffEnd();

	void HealBanEnd();

private:

	UPROPERTY()
	AAOSCharacter* Character;

	UPROPERTY()
	AAOSGameModeBase* GameMode;

	UPROPERTY()
	UAOSAnimInstance* AnimInstance;

	UPROPERTY()
	AAOSController* CharacterController;

	UPROPERTY()
	UItemComponent* ItemComp;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0"))
	float Health = 2500.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0"))
	float MaxHealth = 2500.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0"))
	float Mana = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0"))
	float MaxMana = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0"))
	float Stamina = 200.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0"))
	float MaxStamina = 200.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0"))
	float Defense = 20.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0"))
	float DefaultValue = 5.f;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Player Stats", meta = (ClampMin = "1.0"))
	int32 RandRangeValue = 15;

	float StaminaDecreaseRate = 20.f;

	float StaminaIncreaseRate = 20.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon = nullptr;

	UPROPERTY()
	AWeapon* QuickSlot1Weapon = nullptr;
	UPROPERTY()
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

	UPROPERTY()
	UAudioComponent* VoiceBreathingRunComp = nullptr;

	bool bVoiceLowHealthPlayed = false;
	bool bVoiceLackManaPlayed = false;

	/** ShinbiSword 관련 변수 */

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

	/** 원거리 무기 애니메이션 몽타주 */

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Guns")
	UAnimMontage* GunReloadMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Guns")
	UAnimMontage* GunFireMontage;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Weapons | Guns")
	UAnimMontage* GunHitReactMontage;

	/** Glaive 관련 변수 */

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

	/** 죽음 애니메이션 몽타주 */
	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathMontage;

	bool IsAnimationPlaying = false;

	bool bCanRunning = true;

	float DefaultFOV = 0.f;

	float CurrentFOV = 0.f;

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 50.f;

	UPROPERTY(EditAnywhere)
	float ZoomSpeed = 30.f;

	/** 크로스헤어 퍼짐 지연 타이머 */
	FTimerHandle FireFactorTimer;
	float FireFactorTime = 0.05f;
	bool bFireFactor = false;

	float CrosshairVelocityFactor = 0.f;
	float CrosshairInAirFactor = 0.f;
	float CrosshairZoomFactor = 0.f;
	float CrosshairFireFactor = 0.f;

	FTimerHandle DamageDebuffTimer;
	FTimerHandle HealBanTimer;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bDmgDebuffActivated = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bHealBanActivated = false;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Sign")
	TSubclassOf<UUserWidget> HealBanSignClass;

	UPROPERTY()
	UUserWidget* HealBanSign;

	UPROPERTY(EditAnywhere, Category = "Combat Compnent | Sign")
	TSubclassOf<UUserWidget> AttackDebuffSignClass;

	UPROPERTY()
	UUserWidget* AttackDebuffSign;

public:

	void SetController(AAOSController* Controller);
	void SetGameMode(AAOSGameModeBase* Gamemode);
	void SetAnimInstance(UAOSAnimInstance* AnimInst);
	AWeapon* GetEquippedWeapon() const;
	float GetHealthPercentage() const;
	bool SpendStamina(float StaminaToSpend);
	bool SpendMana(float ManaToSpend);
	bool GetDmgDebuffActivated() const;
	bool GetHealBanActivated() const;
	float GetHealth() const;
	float GetMana() const;
	void SetHealth(float PreHealth);
	void SetMana(float PreMana);
	float GetDefaultValue() const;
	int32 GetRandRangeValue() const;
	void RemoveSign();
};
