
#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyRanged.h"
#include "EnemyRangedSiege.generated.h"

/**
 * �ξ� �հŸ����� ������ ������ ���� �̴Ͼ� Ŭ����
 */

class AProjectile;
class UAnimMontage;

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyRangedSiege : public AEnemyRanged
{
	GENERATED_BODY()

public:

	AEnemyRangedSiege();

	virtual bool CheckRotateToTargetCondition() override;

	/** BT_Task���� ȣ���ϴ� ���� ��� ���� */
	void SiegeModeAttack();

	/** ����� ���� ���� ��ȯ */
	UFUNCTION(BlueprintCallable)
	void ConvertSiegeMode();

	/** ����� ���÷� ��ȯ */
	UFUNCTION(BlueprintCallable)
	void ReleaseSiegeMode();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void HandleStiffAndStun(bool IsHeadShot) override;

	/** ���� ��� ����ü �߻� */
	void SiegeModeProjectileFire();

	void PlaySiegeModeFireMontage();

	void PlaySiegeModeHitReactionMontage();

	UFUNCTION(BlueprintCallable)
	void OnSiegeModeFireMontageEnded();

	UFUNCTION(BlueprintCallable)
	void OnSiegeModeHitReactionMontageEnded();

private:

	UPROPERTY(EditAnywhere, Category = "Enemy | Siege Mode")
	TSubclassOf<AProjectile> SiegeModeProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Enemy | Siege Mode")
	UAnimMontage* SiegeModeFireMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Siege Mode")
	TArray<FName> SiegeModeFireMontageSectionNameArr;

	UPROPERTY(EditAnywhere, Category = "Enemy | Siege Mode")
	UAnimMontage* SiegeModeHitReactionMontage;

};
