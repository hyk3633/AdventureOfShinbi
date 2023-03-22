
#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedWeapon.h"
#include "RangedProjectileWeapon.generated.h"

/**
 * 투사체 스폰 방식의 원거리 무기
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

	/** 발사 함수 */
	virtual void Firing() override;

	/** 산탄 */
	void ScatterFiring(TSubclassOf<AProjectile> Projectile);

	/** 한 발만 발사 */
	void SingleFiring(TSubclassOf<AProjectile> Projectile);

	void SingleFiring();

protected:

	/** 투사체 스폰 */
	void SpawnProjectile(TSubclassOf<AProjectile> Projectile, const FVector& LocToSpawn, const FRotator& RotToSpawn);

	/** 메쉬의 총구 소켓 위치를 저장 */
	void GetSpawnLocation(FVector& ProjLoc);

	/** 메쉬의 총구에서 크로스헤어가 조준하는 방향으로의 Rotation 값을 저장 */
	void GetSpawnRotation(const FVector& ProjLoc, FRotator& ProjRot);

protected:

	/** 산탄 여부 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Scatter")
	bool bScatterGun = false;

	/** 산탄일 경우 탄알 수 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Scatter", meta = (EditCondition = "bScatterGun", ClampMin = "1", ClampMax = "50"))
	int8 NumberOfShots = 10;

	/** 산탄 범위 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Scatter", meta = (EditCondition = "bScatterGun", ClampMin = "0.1", ClampMax = "10.0"))
	float ScatterRange = 1.0f;

private:

	/** 투사체 템플릿 클래스 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Object Pooling")
	UObjectPool* ObjectPooler;

	UPROPERTY(EditAnywhere, Category = "Object Pooling")
	float LifeSpan = 3.f;

};
