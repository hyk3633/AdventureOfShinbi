
#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedWeapon.h"
#include "RangedProjectileWeapon.generated.h"

/**
 * ����ü ���� ����� ���Ÿ� ����
 */

class AProjectile;
class UObjectPool;

UCLASS()
class ADVENTUREOFSHINBI_API ARangedProjectileWeapon : public ARangedWeapon
{
	GENERATED_BODY()
	
public:

	ARangedProjectileWeapon();

	virtual void SetWeaponState(const EWeaponState State) override;

	/** �߻� �Լ� */
	virtual void Firing() override;

	/** ��ź */
	void ScatterFiring(TSubclassOf<AProjectile> Projectile);

	/** �� �߸� �߻� */
	void SingleFiring(TSubclassOf<AProjectile> Projectile);

	void SingleFiring();

protected:

	/** ����ü ���� */
	void SpawnProjectile(TSubclassOf<AProjectile> Projectile, const FVector& LocToSpawn, const FRotator& RotToSpawn);

	/** �޽��� �ѱ� ���� ��ġ�� ���� */
	void GetSpawnLocation(FVector& ProjLoc);

	/** �޽��� �ѱ����� ũ�ν��� �����ϴ� ���������� Rotation ���� ���� */
	void GetSpawnRotation(const FVector& ProjLoc, FRotator& ProjRot);

protected:

	/** ��ź ���� */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Scatter")
	bool bScatterGun = false;

	/** ��ź�� ��� ź�� �� */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Scatter", meta = (EditCondition = "bScatterGun", ClampMin = "1", ClampMax = "50"))
	int8 NumberOfShots = 10;

	/** ��ź ���� */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Scatter", meta = (EditCondition = "bScatterGun", ClampMin = "0.1", ClampMax = "10.0"))
	float ScatterRange = 1.0f;

private:

	/** ����ü ���ø� Ŭ���� */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Object Pooling")
	UObjectPool* ObjectPooler;

	UPROPERTY(EditAnywhere, Category = "Object Pooling")
	float LifeSpan = 3.f;

};
