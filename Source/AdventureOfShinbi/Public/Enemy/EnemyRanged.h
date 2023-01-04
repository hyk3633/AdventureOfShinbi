
#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "EnemyRanged.generated.h"

/**
 * ���Ÿ� ������ ������ �� ĳ����
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

	/** BT_Task���� ȣ���ϴ� ���Ÿ� ���� �Լ� */
	virtual void RangedAttack();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void AbortAttack() override;

	/** ����ü �߻� */
	void ProjectileFire(TSubclassOf<AProjectile> Projectile);

	/** ũ�ν���� ���� Ʈ���̽� */
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

	/** �߻� �ִϸ��̼��� ���� ���� ��� ���� ����� ���� �ִϸ��̼� ��Ÿ�� ���� �̸� �迭 */
	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	TArray<FName> FireMontageSectionNameArr;

	/** ź ���� ���� */
	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack", meta = (ClampMin = 0.f))
	float BulletSpread;

	/** �� ���� ���Ÿ� ���� �� �߻� Ƚ�� */
	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack", meta = (ClampMin = 0.f))
	uint8 FireCount = 3;

	/** ������� �߻�� Ƚ�� */
	uint8 CurrentFireCount = 0;

	/** ���� �ӵ� ���� Ÿ�̸� */
	FTimerHandle FireDelayTimer;

	/** ���� �ӵ� ���� �ð� */
	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	float FireDelayTime = 0.1f;

public:

	AEnemyAIController* GetEnemyController() const;

};
