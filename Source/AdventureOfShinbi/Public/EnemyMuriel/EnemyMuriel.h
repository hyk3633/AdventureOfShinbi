
#pragma once

#include "CoreMinimal.h"
#include "EnemyStrafing/EnemyStrafing.h"
#include "EnemyMuriel.generated.h"

/**
 * �߰� ���� Ŭ����
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

	/** BT_Task���� ȣ���ϴ� �Լ� */

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

	/** �̴Ͼ� ��ȯ ��ų */

	UFUNCTION(BlueprintCallable)
	void Summon();

	bool CheckSpawnPosition(FVector& SafePosition, AActor* CenterActor, float PositionOffset);

	void PlaySummonMontage();

	void PlaySummonParticle(FVector Position);

	UFUNCTION(BlueprintCallable)
	void OnSummonMontageEnded();

	void SummonCoolTimeEnd();

	/** �̴Ͼ� ���� ��ų */

	UFUNCTION(BlueprintCallable)
	void Buff();

	void PlayBuffMontage();

	UFUNCTION(BlueprintCallable)
	void OnBuffMontageEnded();

	void BuffCoolTimeEnd();

	/** �÷��̾�� ������� �Ŵ� ��ų */

	void FireSkillShot();

	void PlaySkillShotFireMontage();

	UFUNCTION(BlueprintCallable)
	void FireSkillShotProjectile();

	UFUNCTION(BlueprintCallable)
	void OnSkillShotFireMontageEnded();

	void SkillShotCoolTimeEnd();

	/** �̴Ͼ��� �÷��̾� ��ó�� �̵� ��Ű�� ��ų */

	void TeleportMinionToPlayer();

	void PlayTeleportMinionEffect();

	void PlayTeleportMinionMontage();

	UFUNCTION(BlueprintCallable)
	void OnTeleportMinionMontageEnded();

	void TeleportMinionDelayEnd();

	void TeleportMinionCoolTimeEnd();

	/** ��ų ���� */
	void SkillFailed();

private:

	/** ��ȯ, ����, �̵� ��ų�� ������ �̴Ͼ��� �����ϴ� �±� */
	FName FriendlyTag;

	/** true �̸� ȭ��Ʈ, false �̸� �� */
	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Main")
	bool bWhite = true;

	/** ���� �ɼ� */

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Buff")
	UAnimMontage* BuffMontage;

	FTimerHandle BuffCoolTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Buff")
	float BuffCoolTime = 60.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Muriel | CoolTime")
	bool bBuffCoolTimeEnd = true;

	/** ��ȯ �ɼ� */

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

	/** ��ų �� �ɼ� */

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

	/** �ڷ���Ʈ �̴Ͼ� */

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Teleport Minion")
	UAnimMontage* TeleportMinionMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Teleport Minion")
	UParticleSystem* TeleportMinionParticle1;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Teleport Minion")
	UParticleSystem* TeleportMinionParticle2;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Teleport Minion")
	USoundCue* TeleportSound;

	/** ĳ���ͷ� ���� ���� �ָ� �ִ� �̴Ͼ� */
	UPROPERTY()
	AActor* FarestMinion = nullptr;

	/** �ڷ���Ʈ ��ų ��ġ�� �����̼� */
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
