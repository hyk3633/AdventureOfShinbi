
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

	/** 소환 스킬 */

	UFUNCTION(BlueprintCallable)
	void Summon();

	bool CheckSpawnPosition(FVector& SafePosition, AActor* CenterActor, float PositionOffset);

	void PlaySummonMontage();

	void PlaySummonParticle(FVector Position);

	UFUNCTION(BlueprintCallable)
	void OnSummonMontageEnded();

	void SummonCoolTimeEnd();

	/** 버프 스킬 */

	UFUNCTION(BlueprintCallable)
	void Buff();

	void PlayBuffMontage();

	UFUNCTION(BlueprintCallable)
	void OnBuffMontageEnded();

	void BuffCoolTimeEnd();

	/** 스킬 샷 스킬 */

	void PlaySkillShotFireMontage();

	UFUNCTION(BlueprintCallable)
	void FireSkillShotProjectile();

	UFUNCTION(BlueprintCallable)
	void OnSkillShotFireMontageEnded();

	void SkillShotCoolTimeEnd();

	/** 텔레포트 미니언 스킬 */

	void TeleportMinionToPlayer();

	void PlayTeleportMinionEffect();

	void PlayTeleportMinionMontage();

	UFUNCTION(BlueprintCallable)
	void OnTeleportMinionMontageEnded();

	void TeleportMinionDelayEnd();

	void TeleportMinionCoolTimeEnd();

private:

	FName FriendlyTag;

	// true 이면 화이트, false 이면 블랙
	UPROPERTY(EditAnywhere, Category = "Main Option")
	bool bWhite = true;

	/** 버프 옵션 */

	UPROPERTY(EditAnywhere, Category = "Buff Option")
	UAnimMontage* BuffMontage;

	FTimerHandle BuffTimer;

	UPROPERTY(EditAnywhere, Category = "Buff Option")
	float BuffCoolTime = 60.f;

	bool bAbleBuff = true;

	/** 소환 옵션 */

	UPROPERTY(EditAnywhere, Category = "Summon Option")
	UAnimMontage* SummonMontage;

	UPROPERTY(EditAnywhere, Category = "Summon Option")
	TArray<TSubclassOf<AEnemyCharacter>> EnemyClassArr;

	UPROPERTY(EditAnywhere, Category = "Summon Option")
	float SpawnDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Summon Option")
	UParticleSystem* SummonParticle;

	int8 BoxTraceDir[8][2] = { {1,0},{1,1},{1,-1},{0,1},{0,-1},{-1,1},{-1,-1},{-1,0} };
	float RotationDir[8] = { 180.f,135.f,-135.f,-90.f,90.f,45.f,-45.f,0.f };

	FTimerHandle SummonCoolTimer;

	UPROPERTY(EditAnywhere, Category = "Summon Option")
	float SummonCoolTime = 60.f;

	bool bAbleSummon = true;

	/** 스킬 샷 옵션 */

	UPROPERTY(EditAnywhere, Category = "SkillShot Option")
	TSubclassOf<AProjectile> SkillProjectileClass;

	UPROPERTY(EditAnywhere, Category = "SkillShot Option")
	UAnimMontage* SkillShotFireMontage;

	UPROPERTY(EditAnywhere, Category = "SkillShot Option")
	float SkillShotDurationTime = 30.f;

	UPROPERTY(EditAnywhere, Category = "SkillShot Option")
	float DmgDecreaseRate = 0.3f;

	FTimerHandle SkillShotCoolTimer;

	UPROPERTY(EditAnywhere, Category = "SkillShot Option")
	float SkillShotCoolTime = 60.f;

	bool bAbleSkillShot = true;

	/** 텔레포트 옵션 : 뮤리엘이 공격당했을 경우 0.1 확률로 미니언 텔레포트, 쿨타임은 1분 */

	UPROPERTY(EditAnywhere, Category = "Teleport Minion Option")
	UAnimMontage* TeleportMinionMontage;

	UPROPERTY(EditAnywhere, Category = "Teleport Minion Option")
	UParticleSystem* TeleportMinionParticle1;

	UPROPERTY(EditAnywhere, Category = "Teleport Minion Option")
	UParticleSystem* TeleportMinionParticle2;

	UPROPERTY(EditAnywhere, Category = "Teleport Minion Option")
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

public:

	bool GetMurielType() const;
	FName GetFriendlyTag() const;
	bool GetAbleBuff() const;
	bool GetAbleSummon() const;
	bool GetAbleSkillShot() const;
	float GetSkillShotDurationTime() const;
};
