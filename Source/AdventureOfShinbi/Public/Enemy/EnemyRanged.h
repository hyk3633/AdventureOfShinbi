
#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "EnemyRanged.generated.h"

/**
 * 원거리 공격이 가능한 적 캐릭터
 */

class AProjectile;
class UAnimMontage;
class AEnemyAIController;
class UParticleSystem;

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyRanged : public AEnemyCharacter
{
	GENERATED_BODY()

public:

	AEnemyRanged();

	/** BT_Task에서 호출하는 원거리 공격 함수 */
	virtual void RangedAttack();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void AbortAttack() override;

	/** 투사체 발사 */
	void ProjectileFire(TSubclassOf<AProjectile> Projectile);

	/** 크로스헤어 라인 트레이스 */
	void CrosshairLineTrace(FVector& OutHitPoint);

	virtual void PlayFireMontage();

	UFUNCTION(BlueprintCallable)
	virtual void OnFireMontageEnded();

	void FinishFire();

	void AfterFireDelay();

protected:

	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	UAnimMontage* FireMontage;

private:

	/** 발사 애니메이션이 여러 개인 경우 랜덤 재생을 위한 애니메이션 몽타주 섹션 이름 배열 */
	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	TArray<FName> FireMontageSectionNameArr;

	/** 탄 퍼짐 정도 */
	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack", meta = (ClampMin = 0.f))
	float BulletSpread;

	/** 한 번의 원거리 공격 당 발사 횟수 */
	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack", meta = (ClampMin = 0.f))
	uint8 FireCount = 3;

	/** 현재까지 발사된 횟수 */
	uint8 CurrentFireCount = 0;

	/** 연사 속도 제어 타이머 */
	FTimerHandle FireDelayTimer;

	/** 연사 속도 지연 시간 */
	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	float FireDelayTime = 0.1f;

public:

	AEnemyAIController* GetEnemyController() const;

};
