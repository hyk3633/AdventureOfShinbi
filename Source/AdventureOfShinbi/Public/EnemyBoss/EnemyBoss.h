
#pragma once

#include "CoreMinimal.h"
#include "EnemyStrafing/EnemyStrafing.h"
#include "Types/WeaponType.h"
#include "EnemyBoss.generated.h"

/**
 * 
 */

class AAOSCharacter;
class ABossAIController;
class UBoxComponent;
class UAnimMontage;
class UParticleSystem;
class USoundCue;

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

	void Blizzard();

	void Dash();

	void CreateIceWall();

	void BackAttack();

	void Evade();

	void EmitSwordAura();

	void Freezing();

	void IcicleAttack();

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

	virtual void OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	virtual void OnDashBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void DetectAttack();

	virtual void Weapon1LineTrace() override;

	virtual void GetLineTraceHitResult(FHitResult& HitResult) override;

	virtual bool CheckStrafingCondition() override;

	virtual void ChangeStrafingDirection() override;

	/** 공격 */

	virtual void PlayAttackMontage() override;

	UFUNCTION(BlueprintCallable)
	void MoveWhileAttack();

	virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;

	virtual void StopAttackMontage() override;

	void PlayAttack1Montage();
	void PlayAttack2Montage();
	void PlayAttack3Montage();

	/** 눈보라 공격 */

	UFUNCTION(BlueprintCallable)
	void CheckTargetInBlizzardRange();

	void PlayBlizzardMontage();

	void ApplyBlizzardDebuff();

	void BlizzardDotDamage(float DeltaTime);

	void BlizzardDebuffEnd();

	UFUNCTION(BlueprintCallable)
	void BlizzardMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	void BlizzardCoolTimeEnd();

	/** 대쉬 */

	void PlayDashMontage();

	UFUNCTION(BlueprintCallable)
	void DashLaunch();

	UFUNCTION(BlueprintCallable)
	void DashMontageEnd();

	void SetBoxState(EBoxState State);

	void DashCoolTimeEnd();

	/** 얼음 벽 */

	void IcaWallDisappearAndCheck();

	void SetRotationToAppear(FName Direction);

	void IceWallCoolTimeEnd();

	void PlayIceWallAttackMontage();

	UFUNCTION(BlueprintCallable)
	void IceWallAttackMontageEnd(bool IsSuccess);

	/** 뒤잡기 */

	void BackAttackDisappearAndCheck();

	bool CheckObstacle(AActor* CenterActor, FName Direction, float Offset);

	void AppearFromLRB();

	void PlayBackAttackMontage();

	UFUNCTION(BlueprintCallable)
	void BackAttackMontageEnd(bool IsSuccess);

	void BackAttackCoolTimeEnd();

	/** 회피 */

	// bool CheckObstacle(FName Dir);

	void PlayEvadeMontage(FName Dir);

	UFUNCTION(BlueprintCallable)
	void EvadeMontageEnd(bool IsSuccess);

	void EvadeCoolTimeEnd();

	/** 검기 발사 */

	UFUNCTION(BlueprintCallable)
	void ProjectileFire();
		
	virtual void PlayFireMontage() override;

	virtual void OnFireMontageEnded() override;

	void EmitSwordAuraCoolTimeEnd();

	/** 빙결 */

	void FreezingActivate();

	void PlayFreezingMontage();

	void FreezingDurationEnd();

	UFUNCTION(BlueprintCallable)
	void FreezingMontageEnded();

	void FreezingCoolTimeEnd();

	/** 고드름 */

	UFUNCTION(BlueprintCallable)
	void RisingIcicle();

	UFUNCTION(BlueprintCallable)
	void SetIcicleLocation();

	UFUNCTION(BlueprintCallable)
	void BurstShockWave();

	void PlayIcicleAttackMontage();

	UFUNCTION(BlueprintCallable)
	void IcicleAttackMontageEnded();

	void IcicleAttackCoolTimeEnd();

	UFUNCTION(BlueprintCallable)
	void ActivateBurst();

	UFUNCTION(BlueprintCallable)
	void DeactivateBurst();

private:

	AAOSCharacter* Target;

	ABossAIController* BossController;

	bool bPhase2 = false;

	/** 콤보 공격 */

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* Attack2Montage;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* Attack3Montage;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float BurstDamage = 100.f;

	/** 눈보라 공격 */

	UPROPERTY(EditAnywhere, Category = "Blizzard")
	UAnimMontage* BlizzardMontage;

	bool bBlizzardDebuffOn = false;

	FTimerHandle BlizzardDebuffTimer;
	float BlizzardDebuffTime = 10.f;

	FTimerHandle BlizzardCoolTimer;
	float BlizzardCoolTime = 30.f;
	bool bBlizzardCoolTimeEnd = true;

	/** 대쉬 */

	UPROPERTY(EditAnywhere, Category = "Dash")
	UBoxComponent* DashAndWallBox;

	UPROPERTY(EditAnywhere, Category = "Dash")
	UAnimMontage* DashMontage;

	FVector DashStartLocation;

	FTimerHandle DashDelayTimer;
	float DashDelayTime = 1.f;

	FTimerHandle DashCoolTimer;
	float DashCoolTime = 20.f;
	bool bDashCoolTimeEnd = true;

	UPROPERTY(VisibleAnywhere)
	int8 DashCount = 0;
	UPROPERTY(VisibleAnywhere)
	int8 MaxDashCount = 3;

	/** 얼음 벽 */

	UPROPERTY(EditAnywhere, Category = "IceWall")
	UParticleSystem* IceWallParticle;

	UPROPERTY(EditAnywhere, Category = "IceWall")
	UAnimMontage* IceWallAttackMontage;

	bool bAbleIceWall = false;

	FTimerHandle IceWallAttackDisappearTimer;
	float IceWallAttackDisappearTime = 2.f;

	FTimerHandle IceWallAttackAppearTimer;
	float IceWallAttackAppearTime = 0.45f;

	FTimerHandle IceWallCoolTimer;
	float IceWallCoolTime = 40.f;
	bool bIceWallCoolTimeEnd = true;

	/** 뒤잡기 */

	UPROPERTY(EditAnywhere, Category = "Back Attack")
	UAnimMontage* BackAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Back Attack")
	UParticleSystem* DecoyParticle;

	UPROPERTY(EditAnywhere, Category = "Back Attack")
	UParticleSystem* DecoyDestroyParticle;

	UPROPERTY(EditAnywhere, Category = "Back Attack")
	UParticleSystem* AppearanceParticle;

	UPROPERTY(EditAnywhere, Category = "Back Attack")
	USoundCue* DisappearSound;

	UPROPERTY(EditAnywhere, Category = "Back Attack")
	USoundCue* AppearSound;

	UPROPERTY(EditAnywhere, Category = "Back Attack")
	USoundCue* DecoyDestroySound;

	bool bAbleBackAttack = false;

	FVector LocationToAppear;
	FRotator RotationToAppear;

	FTimerHandle BackAttackAppearTimer;
	float BackAttackAppearTime = 0.45f;

	FTimerHandle DisappearTimer;
	float DisappearTime = 1.5f;

	FTimerHandle BackAttackCoolTimer;
	float BackAttackCoolTime = 30.f;
	bool bBackAttackCoolTimeEnd = true;

	/** 회피 */

	UPROPERTY(EditAnywhere, Category = "Evade")
	UAnimMontage* EvadeMontage;

	bool bAbleEvade = false;

	FTimerHandle EvadeCoolTimer;
	float EvadeCoolTime = 20.f;
	bool bEvadeCoolTimeEnd = true;

	/** 검기 발사 */

	UPROPERTY(EditAnywhere, Category = "Emit Sword Aura")
	UAnimMontage* EmitSwordAuraMontage;

	FTimerHandle EmitSwordAuraCoolTimer;
	float EmitSwordAuraCoolTime = 10.f;
	bool bEmitSwordAuraCoolTimeEnd = true;

	/** 빙결 */

	UPROPERTY(EditAnywhere, Category = "Freezing")
	UAnimMontage* FreezingMontage;

	UPROPERTY(EditAnywhere, Category = "Freezing")
	UParticleSystem* TargetFreezingParticle;

	UPROPERTY(EditAnywhere, Category = "Freezing")
	USoundCue* FreezingSound;

	bool bAbleFreezing = false;

	FTimerHandle FreezingDurationTimer;
	float FreezingDurationTime = 6.f;

	FTimerHandle FreezingCoolTimer;
	float FreezingCoolTime = 40.f;
	bool bFreezingCoolTimeEnd = true;

	/** 고드름 */

	UPROPERTY(EditAnywhere, Category = "Icicle Attack")
	UAnimMontage* IcicleAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Icicle Attack")
	UParticleSystem* IcicleParticle;

	UPROPERTY(EditAnywhere, Category = "Icicle Attack")
	USoundCue* IcicleSound;

	bool bActivateBurst = false;

	FVector IcicleLocation;

	FTimerHandle IcicleAttackCoolTimer;
	float IcicleAttackCoolTime = 30.f;
	bool bIcicleAttackCoolTimeEnd = true;

public:

	APawn* GetTarget() const;
	bool CalculPercentage(float Percentage);
	bool GetPhase2() const;
	bool GetAbleIceWall() const;
	bool GetAbleBackAttack() const;
	bool GetAbleEvade() const;
	bool GetAbleFreezing() const;
	bool GetBlizzardCoolTimeEnd() const;
	bool GetEmitSwordAuraCoolTimeEnd() const;
	bool GetDashCoolTimeEnd() const;
	bool GetIcicleAttackCoolTimeEnd() const;
};
