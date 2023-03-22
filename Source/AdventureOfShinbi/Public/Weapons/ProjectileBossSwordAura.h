
#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileHoming.h"
#include "ProjectileBossSwordAura.generated.h"

/**
 * ���� ��ų ����ü : �÷��̾�� ���� �� �̵� �ӵ� ���Ҹ� ����
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

	/** �÷��̾� �ӵ� ���� Ÿ�̸� */
	FTimerHandle SlowTimer;
	
	/** �÷��̾� �ӵ� ���� �ð� */
	UPROPERTY(EditAnywhere)
	float SlowTime = 5.f;

};
