
#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyRanged.h"
#include "EnemyRangedSiege.generated.h"

/**
 * 훨씬 먼거리에서 공격이 가능한 시즈 미니언 클래스
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

	/** BT_Task에서 호출하는 시즈 모드 공격 */
	void SiegeModeAttack();

	/** 모션을 시즈 모드로 전환 */
	UFUNCTION(BlueprintCallable)
	void ConvertSiegeMode();

	/** 모션을 평상시로 전환 */
	UFUNCTION(BlueprintCallable)
	void ReleaseSiegeMode();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void HandleStiffAndStun(bool IsHeadShot) override;

	/** 시즈 모드 투사체 발사 */
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
