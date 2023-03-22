
#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileHoming.h"
#include "ProjectileBossSwordAura.generated.h"

/**
 * 보스 스킬 투사체 : 플레이어에게 적중 시 이동 속도 감소를 유발
 */

class AAOSCharacter;

UCLASS()
class ADVENTUREOFSHINBI_API AProjectileBossSwordAura : public AProjectileHoming
{
	GENERATED_BODY()
	
public:

	AProjectileBossSwordAura();

protected:

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void SlowTimeEnd();

private:

	UPROPERTY()
	AAOSCharacter* Target;

	/** 플레이어 속도 감소 타이머 */
	FTimerHandle SlowTimer;
	
	/** 플레이어 속도 감소 시간 */
	UPROPERTY(EditAnywhere)
	float SlowTime = 5.f;

};
