
#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 근접 무기 클래스
 */

class UParticleSystem;
class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	AMeleeWeapon();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** 무기의 캡슐 트레이스 결과에 따라 데미지 적용 및 효과 재생 */
	virtual bool WeaponCapsuleTrace();

	/** 캡슐 트레이스 검사 후 HitResult를 저장 */
	bool GetCapsuleTraceHitResult(TArray<FHitResult>& HitResults);

	void PlayAttackEffect(const FVector& Location, const FRotator& Rotation);

protected:

	/** 무기의 타격 판정을 결정하는 캡슐 트레이스의 반지름 */
	float CapsuleRadius = 0.f;

	/** 무기의 타격 판정을 결정하는 캡슐 트레이스의 중심에서 캡슐 끝 부분 까지의 거리 */
	float CapsuleHalfHeight = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
	USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
	TSubclassOf<UCameraShakeBase> CameraShakeHitImpact;

private:

	/** 무기의 캡슐 트레이스를 활성화 */
	bool bActivateWeaponTrace = false;

public:

	void ActivateWeaponTrace();
	void DeactivateWeaponTrace();

};
