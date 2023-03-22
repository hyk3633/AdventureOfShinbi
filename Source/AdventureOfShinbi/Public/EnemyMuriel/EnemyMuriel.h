
#pragma once

#include "CoreMinimal.h"
#include "EnemyStrafing/EnemyStrafing.h"
#include "EnemyMuriel.generated.h"

/**
 * 중간 보스 클래스
 */

class AEnemyCharacter;
class UParticleSystem;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMurielDeathDelegate);

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyMuriel : public AEnemyStrafing
{
	GENERATED_BODY()

public:

	AEnemyMuriel();

	/** BT_Task에서 호출하는 함수 */

	virtual void RangedAttack() override;

	void SummonMinion();

	void ProvideBuff();

	void FindTeleportPosition();

	UPROPERTY(BlueprintAssignable)
	FMurielDeathDelegate DMurielDeath;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void HandleStiffAndStun(bool IsHeadShot) override;

	virtual void ResetAIState() override;

	virtual void HandleHealthChange(float DamageReceived) override;

	/** 미니언 소환 스킬 */

	UFUNCTION(BlueprintCallable)
	void Summon();

	bool CheckSpawnPosition(FVector& SafePosition, AActor* CenterActor, float PositionOffset);

	void PlaySummonMontage();

	void PlaySummonParticle(FVector Position);

	UFUNCTION(BlueprintCallable)
	void OnSummonMontageEnded();

	void SummonCoolTimeEnd();

	/** 미니언 버프 스킬 */

	UFUNCTION(BlueprintCallable)
	void Buff();

	void PlayBuffMontage();

	UFUNCTION(BlueprintCallable)
	void OnBuffMontageEnded();

	void BuffCoolTimeEnd();

	/** 플레이어에게 디버프를 거는 스킬 */

	void FireSkillShot();

	void PlaySkillShotFireMontage();

	UFUNCTION(BlueprintCallable)
	void FireSkillShotProjectile();

	UFUNCTION(BlueprintCallable)
	void OnSkillShotFireMontageEnded();

	void SkillShotCoolTimeEnd();

	/** 미니언을 플레이어 근처로 이동 시키는 스킬 */

	void TeleportMinionToPlayer();

	void PlayTeleportMinionEffect();

	void PlayTeleportMinionMontage();

	UFUNCTION(BlueprintCallable)
	void OnTeleportMinionMontageEnded();

	void TeleportMinionDelayEnd();

	void TeleportMinionCoolTimeEnd();

	/** 스킬 실패 */
	void SkillFailed();

private:

	/** 소환, 버프, 이동 스킬을 적용할 미니언을 구분하는 태그 */
	FName FriendlyTag;

	/** true 이면 화이트, false 이면 블랙 */
	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Main")
	bool bWhite = true;

	/** 버프 옵션 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Buff")
	UAnimMontage* BuffMontage;

	FTimerHandle BuffCoolTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Buff")
	float BuffCoolTime = 60.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Muriel | CoolTime")
	bool bBuffCoolTimeEnd = true;

	/** 소환 옵션 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Summon")
	UAnimMontage* SummonMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Summon")
	TArray<TSubclassOf<AEnemyCharacter>> EnemyClassArr;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Summon")
	float SpawnDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Summon")
	UParticleSystem* SummonParticle;

	FTimerHandle SummonCoolTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Summon Option")
	float SummonCoolTime = 60.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Muriel | CoolTime")
	bool bSummonCoolTimeEnd = true;

	/** 스킬 샷 옵션 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | SkillShot")
	TSubclassOf<AProjectile> SkillProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | SkillShot")
	UAnimMontage* SkillShotFireMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | SkillShot")
	float SkillShotDurationTime = 10.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | SkillShot")
	float DmgDecreaseRate = 0.3f;

	FTimerHandle SkillShotCoolTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | SkillShot")
	float SkillShotCoolTime = 60.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Muriel | CoolTime")
	bool bSkillShotCoolTimeEnd = true;

	/** 텔레포트 미니언 */

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Teleport Minion")
	UAnimMontage* TeleportMinionMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Teleport Minion")
	UParticleSystem* TeleportMinionParticle1;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Teleport Minion")
	UParticleSystem* TeleportMinionParticle2;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Teleport Minion")
	USoundCue* TeleportSound;

	/** 캐릭터로 부터 가장 멀리 있는 미니언 */
	UPROPERTY()
	AActor* FarestMinion = nullptr;

	/** 텔레포트 시킬 위치와 로테이션 */
	FVector TeleportPosition = FVector::ZeroVector;
	FRotator TeleportRotation = FRotator::ZeroRotator;

	int8 RotationIdx = 0;

	FTimerHandle TeleportTimer;
	float TeleportDelayTime = 1.f;

	FTimerHandle TeleportCoolTimer;
	float TeleportCoolTime = 40.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Muriel | CoolTime")
	bool bTeleportMinionCoolTimeEnd = true;

public:

	FName GetFriendlyTag() const;
	bool GetMurielType() const;
	bool GetBuffCoolTimeEnd() const;
	bool GetSummonCoolTimeEnd() const;
	bool GetTeleportMinionCoolTimeEnd() const;
	float GetSkillShotDurationTime() const;
};
