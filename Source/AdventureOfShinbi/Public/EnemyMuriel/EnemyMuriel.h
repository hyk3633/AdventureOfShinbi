
#pragma once

#include "CoreMinimal.h"
#include "EnemyStrafing/EnemyStrafing.h"
#include "EnemyMuriel.generated.h"

/**
 * 
 */

class AEnemyCharacter;
class UParticleSystem;
class USoundCue;

DECLARE_MULTICAST_DELEGATE(FOnSkillEndDelegate);

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyMuriel : public AEnemyStrafing
{
	GENERATED_BODY()

public:

	AEnemyMuriel();

	void ProvideBuff();

	void SummonMinion();

	void FireSkillShot();

	void FindTeleportPosition();

	FOnSkillEndDelegate OnSkillEnd;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

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

	virtual void HandleStiffAndStun(FName& BoneName) override;

	/** ��ȯ ��ų */

	UFUNCTION(BlueprintCallable)
	void Summon();

	bool CheckSpawnPosition(FVector& SafePosition, AActor* CenterActor, float PositionOffset);

	void PlaySummonMontage();

	void PlaySummonParticle(FVector Position);

	UFUNCTION(BlueprintCallable)
	void OnSummonMontageEnded();

	void SummonCoolTimeEnd();

	/** ���� ��ų */

	UFUNCTION(BlueprintCallable)
	void Buff();

	void PlayBuffMontage();

	UFUNCTION(BlueprintCallable)
	void OnBuffMontageEnded();

	void BuffCoolTimeEnd();

	void BuffDurationEnd();

	void PlayBuffParticle(const ACharacter* BuffTarget);

	/** ��ų �� ��ų */

	void PlaySkillShotFireMontage();

	UFUNCTION(BlueprintCallable)
	void FireSkillShotProjectile();

	UFUNCTION(BlueprintCallable)
	void OnSkillShotFireMontageEnded();

	void SkillShotCoolTimeEnd();

	/** �ڷ���Ʈ �̴Ͼ� ��ų */

	void TeleportMinionToPlayer();

	void PlayTeleportMinionEffect();

	void PlayTeleportMinionMontage();

	UFUNCTION(BlueprintCallable)
	void OnTeleportMinionMontageEnded();

	void TeleportMinionDelayEnd();

	void TeleportMinionCoolTimeEnd();

private:

	FName FriendlyTag;

	// true �̸� ȭ��Ʈ, false �̸� ��
	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Main")
	bool bWhite = true;

	bool bIsCasting = false;

	/** ���� �ɼ� */

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Buff")
	UAnimMontage* BuffMontage;

	TArray<AActor*> TargetsToBuff;

	FTimerHandle BuffDurationTimer;
	float BuffDurationTime = 20.f;

	FTimerHandle BuffCoolTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Buff")
	float BuffCoolTime = 60.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Muriel | CoolTime")
	bool bBuffCoolTimeEnd = true;
	
	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Buff")
	UParticleSystem* BuffStartParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Buff")
	UParticleSystem* BuffParticle;

	UParticleSystemComponent* BuffParticleComponent;
	TArray<UParticleSystemComponent*> BuffParticleComponents;

	/** ��ȯ �ɼ� */

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Summon")
	UAnimMontage* SummonMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Summon")
	TArray<TSubclassOf<AEnemyCharacter>> EnemyClassArr;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Summon")
	float SpawnDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Muriel | Summon")
	UParticleSystem* SummonParticle;

	int8 BoxTraceDir[8][2] = { {1,0},{1,1},{1,-1},{0,1},{0,-1},{-1,1},{-1,-1},{-1,0} };
	float RotationDir[8] = { 180.f,135.f,-135.f,-90.f,90.f,45.f,-45.f,0.f };

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
	float SkillShotDurationTime = 30.f;

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

	AActor* FarestMinion = nullptr;
	FTransform TeleportTransform;
	FVector TeleportPosition = FVector::ZeroVector;
	FRotator TeleportRotation = FRotator::ZeroRotator;
	int8 RotationIdx = 0;

	FTimerHandle TeleportTimer;
	float TeleportDelayTime = 1.f;

	FTimerHandle TeleportCoolTimer;
	float TeleportCoolTime = 40.f;

	bool bAbleTeleportMinion = true;
	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Muriel | CoolTime")
	bool bTeleportMinionCoolTimeEnd = true;

public:

	bool GetMurielType() const;
	bool GetIsCasting() const;
	FName GetFriendlyTag() const;
	bool GetBuffCoolTimeEnd() const;
	bool GetSummonCoolTimeEnd() const;
	bool GetSkillShotCoolTimeEnd() const;
	float GetSkillShotDurationTime() const;
};
