
#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedWeapon.h"
#include "RangedHitScanWeapon.generated.h"

/**
 * 히트 스캔 방식의 원거리 무기
 */

class ACasing;

UCLASS()
class ADVENTUREOFSHINBI_API ARangedHitScanWeapon : public ARangedWeapon
{
	GENERATED_BODY()

public:

	ARangedHitScanWeapon();

	/** 발사 함수 */
	virtual void Firing() override;

protected:

	/** 탄피 스폰 */
	void SpawnCasing();

	/** 산탄 */
	void ScatterFiring();

	/** 한 발만 발사 */
	void SingleFiring();

	/** 메쉬의 소켓 위치 저장 */
	void GetTraceStart(FVector& Start);

	/** 크로스헤어 라인트레이스의 적중 위치 저장 */
	FVector GetTraceEnd(const FVector& Start);

	/** 궤적 파티클 재생 */
	void DrawTrailParticle(const FVector StartPoint, const FVector EndPoint);

	/** 트레이스 결과에 따른 처리 수행 */
	void ProcessHitResult(const FHitResult& HitResult);

private:

	/** 탄피 템플릿 클래스 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	TSubclassOf<ACasing> CasingClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	UParticleSystem* TargetHitParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	UParticleSystem* WorldHitParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	UParticleSystem* TrailParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	USoundCue* TargetHitSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	USoundCue* WorldHitSound;

	/** 탄 퍼짐 정도 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Attributes", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float BulletSpread = 0.f;

	/** 산탄 여부 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Attributes")
	bool bScatter = false;

	/** 산탄일 경우 탄알 수 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Attributes", meta = (EditCondition = "bScatterGun", ClampMin = "1", ClampMax = "50"))
	int8 NumberOfShots = 10;

	/** 크로스헤어 트레이스 적중 위치 */
	FVector HitPoint;

	/** 트레이스 적중 위치의 Rotation */
	FRotator ImpactRotator;

};
