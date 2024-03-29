
#pragma once

#include "CoreMinimal.h"
#include "EnemyStrafing/EnemyStrafing.h"
#include "Types/WeaponType.h"
#include "EnemyBoss.generated.h"

/**
 * 최종 보스 클래스
 */

class AAOSCharacter;
class ABossAIController;
class AAOSController;
class UBoxComponent;
class UAnimMontage;
class UParticleSystem;
class USoundCue;
class UAudioComponent;

DECLARE_DELEGATE(BossDefeatDelegate);

/** 콜리전 박스 컴포넌트 상태 enum */
UENUM()
enum class EBoxState : uint8
{
	EBS_Disabled,
	EBS_Dash,
	EBS_Wall,

	EBS_MAX
};

UCLASS(Blueprintable)
class ADVENTUREOFSHINBI_API AEnemyBoss : public AEnemyStrafing
{
	GENERATED_BODY()
	
public:

	AEnemyBoss();

	/** BGM 재생 및 공격 대상 설정 */
	void SetTarget();

	/** BT_Task에서 호출하는 함수 */

	virtual void Attack() override;

	virtual void RangedAttack() override;

	void EvadeSkill();

	BossDefeatDelegate DBossDefeat;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void RotateToTarget(float DeltaTime) override;

	virtual void TakePointDamage
	(
		AActor* DamagedActor,
		float DamageReceived,
		class AController* InstigatedBy,
		FVector HitLocation,
		class UPrimitiveComponent* FHitComponent,
		FName BoneName,
		FVector ShotFromDirection,
		const class UDamageType* DamageType,
		AActor* DamageCauser
	) override;

	virtual void SetHealthBar() override;

	virtual void OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	virtual void OnDashBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void DetectAttack();

	virtual void ResetAIState() override;

	virtual bool Weapon1BoxTrace() override;

	virtual void DoStrafing() override;

	virtual bool CheckStrafingCondition() override;

	virtual void ChangeStrafingDirection() override;

	void RangedAttackCoolTimeEnd();

	void EvadeSkillCoolTimeEnd();

	/** 공격 애니메이션에서 캐릭터가 공중에 뜰 수 있도록 설정 */
	UFUNCTION(BlueprintCallable)
	void SetFlying();

	virtual void PlayDeathMontage() override;

	virtual void DeathMontageEnded() override;

	/** 공격 */

	virtual void PlayAttackMontage() override;

	UFUNCTION(BlueprintCallable)
	void MoveWhileAttack();

	virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;

	virtual void StopAttackMontage() override;

	void PlayAttack1Montage();
	void PlayAttack2Montage();
	void PlayAttack3Montage();

	UFUNCTION(BlueprintCallable)
	void DisableCollision();

	UFUNCTION(BlueprintCallable)
	void EnableCollision();

	/** 스킬 : 빙결 (플레이어 행동 불능) */

	void Freezing();

	void FreezingActivate();

	void PlayFreezingMontage();

	void FreezingDurationEnd();

	UFUNCTION(BlueprintCallable)
	void FreezingMontageEnded();

	void FreezingCoolTimeEnd();

	/** 스킬 : 눈보라 공격 (플레이어 이동 속도 감소) */

	void Blizzard();

	UFUNCTION(BlueprintCallable)
	void CheckTargetInBlizzardRange();

	void PlayBlizzardMontage();

	void ApplyBlizzardDebuff();

	void BlizzardDotDamage(float DeltaTime);

	void BlizzardDebuffEnd();

	UFUNCTION(BlueprintCallable)
	void BlizzardMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	/** 스킬 : 대쉬 (플레이어를 향해 돌진하며 데미지를 줌) */

	void Dash();

	void PlayDashMontage();

	UFUNCTION(BlueprintCallable)
	void DashLaunch();

	UFUNCTION(BlueprintCallable)
	void DashMontageEnd();

	void SetBoxState(EBoxState State);

	/** 스킬 : 검기 발사 (플레이어 적중 시 이동 속도 감소) */

	void EmitSwordAura();

	UFUNCTION(BlueprintCallable)
	void FireSwordAura();

	virtual void PlayFireMontage() override;

	virtual void OnFireMontageEnded() override;

	/** 스킬 : 고드름 (플레이어를 공중에 띄우며 데미지를 줌) */

	void IcicleAttack();

	UFUNCTION(BlueprintCallable)
	void RisingIcicle();

	UFUNCTION(BlueprintCallable)
	void SetIcicleLocation();

	UFUNCTION(BlueprintCallable)
	void BurstShockWave();

	void PlayIcicleAttackMontage();

	UFUNCTION(BlueprintCallable)
	void IcicleAttackMontageEnded();

	UFUNCTION(BlueprintCallable)
	void ActivateBurst();

	UFUNCTION(BlueprintCallable)
	void DeactivateBurst();

	/** 스킬 : 회피 (플레이어의 공격 회피) */

	void Evade();

	void PlayEvadeMontage(FName Dir);

	UFUNCTION(BlueprintCallable)
	void EvadeMontageEnd(bool IsSuccess);

	/** 스킬 : 뒤잡기 (플레이어의 뒤에서 기습 공격) */

	void BackAttack();

	void BackAttackDisappearAndCheck();

	bool CheckObstacle(AActor* CenterActor, FName Direction, float Offset);

	void AppearFromLRB();

	void PlayBackAttackMontage();

	UFUNCTION(BlueprintCallable)
	void BackAttackMontageEnd(bool IsSuccess);

	/** 스킬 : 얼음 벽 (플레이어의 공격을 차단하는 벽 생성 후 무작위 위치에서 기습 공격) */

	void CreateIceWall();

	void IcaWallDisappearAndCheck();

	void SetRotationToAppear(FName Direction);

	void PlayIceWallAttackMontage();

	UFUNCTION(BlueprintCallable)
	void IceWallAttackMontageEnd(bool IsSuccess);

	virtual void PlayerDead() override;

private:

	UPROPERTY()
	AAOSCharacter* Target;

	UPROPERTY()
	ABossAIController* BossController;

	UPROPERTY()
	AAOSController* PlayerController;

	/** 페이즈 2 여부 */
	bool bPhase2 = false;

	float StrafingSpeed = 450.f;

	FTimerHandle FreezingCoolTimer;
	float FreezingCoolTime = 10.f;
	bool bFreezingCoolTimeEnd = false;

	FTimerHandle RangedAttackCoolTimer;
	bool bRangedAttackCoolTimeEnd = false;
	float RangedAttackCoolTime = 10.f;

	FTimerHandle EvadeSkillCoolTimer;
	bool bEvadeSkillCoolTimeEnd = false;
	float EvadeSkillCoolTime = 10.f; 

	bool bAbleEvadeSkill = false;
	int8 EvadeSkillNum = -1;

	FTimerHandle AttackDelayTimer;

	int8 AttackNum = -1;

	int8 RangedAttackNum = -1;

	/** 소멸 효과 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Death")
	UParticleSystem* DissolveParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Death")
	UParticleSystem* DissolveSound;

	/** 콤보 공격 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	UAnimMontage* Attack2Montage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	UAnimMontage* Attack3Montage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	float BurstDamage = 800.f;

	/** 빙결 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Freezing")
	UAnimMontage* FreezingMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Freezing")
	UParticleSystem* TargetFreezingParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Freezing")
	USoundCue* FreezingSound;

	FTimerHandle FreezingDurationTimer;
	float FreezingDurationTime = 4.5f;

	bool bFreezingAttack = false;

	bool bFreezingHitted = false;

	/** 눈보라 공격 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Blizzard")
	UAnimMontage* BlizzardMontage;

	bool bBlizzardDebuffOn = false;

	FTimerHandle BlizzardDebuffTimer;
	float BlizzardDebuffTime = 7.f;

	/** 대쉬 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Dash")
	UBoxComponent* DashAndWallBox;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Dash")
	UAnimMontage* DashMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Dash")
	USoundCue* DashHitSound;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Dash")
	TSubclassOf<UCameraShakeBase> CameraShakeDash;

	FVector DashStartLocation;

	FTimerHandle DashDelayTimer;
	float DashDelayTime = 1.f;

	int8 DashCount = 0;
	int8 MaxDashCount = 3;

	bool bDashAttack = false;

	/** 검기 발사 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Emit Sword Aura")
	UAnimMontage* EmitSwordAuraMontage;

	/** 고드름 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Icicle Attack")
	UAnimMontage* IcicleAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Icicle Attack")
	UParticleSystem* IcicleParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Icicle Attack")
	USoundCue* IcicleSound;

	bool bActivateBurst = false;

	bool bIcicleAttack = false;

	FVector IcicleLocation;

	/** 회피 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Evade")
	UAnimMontage* EvadeMontage;

	/** 뒤잡기 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Back Attack")
	UAnimMontage* BackAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Back Attack")
	UParticleSystem* DecoyParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Back Attack")
	UParticleSystem* DecoyDestroyParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Back Attack")
	UParticleSystem* AppearanceParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Back Attack")
	USoundCue* DisappearSound;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Back Attack")
	USoundCue* AppearSound;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Back Attack")
	USoundCue* DecoyDestroySound;

	bool bAbleBackAttack = false;

	FVector LocationToAppear;
	FRotator RotationToAppear;

	FTimerHandle BackAttackAppearTimer;
	float BackAttackAppearTime = 0.45f;

	FTimerHandle DisappearTimer;
	float DisappearTime = 1.5f;

	/** 얼음 벽 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | IceWall")
	UParticleSystem* IceWallParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | IceWall")
	UAnimMontage* IceWallAttackMontage;

	bool bAbleIceWall = false;

	FTimerHandle IceWallAttackDisappearTimer;
	float IceWallAttackDisappearTime = 2.f;

	FTimerHandle IceWallAttackAppearTimer;
	float IceWallAttackAppearTime = 0.45f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Music")
	USoundCue* BattleMusic;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Music")
	USoundCue* Phase2Music;

	UPROPERTY()
	UAudioComponent* MusicComp;

	UPROPERTY(EditAnywhere, Category = "Enemy | Boss | Music")
	float Volume = 1.f;

	TArray<FName> CheckDirectionArr;

public:

	bool CalculPercentage(float Percentage);
	bool GetPhase2() const;
	bool GetRangedAttackCoolTimeEnd() const;
	bool GetAbleEvadeSkill() const;
	bool GetDashAttack() const;
	bool GetIcicleAttack() const;
};
