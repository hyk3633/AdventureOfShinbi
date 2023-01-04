
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class URadialForceComponent;

/**
* 투사체 기본 클래스 
*/

UCLASS()
class ADVENTUREOFSHINBI_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AProjectile();

	/** 발사하는 주체에서 데미지 설정 */
	void SetDamage(float Value);

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	/** 충돌 감지 컴포넌트 */
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision;

	/** 투사체의 움직임을 담당하는 컴포넌트 */
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	/** true 일 경우 RadialDamage를 적용 */
	UPROPERTY(EditAnywhere, Category = "Projectile | Setting")
	bool bIsExplosive = false;

	/** true 일 경우 플레이어가 스폰하는 투사체 */
	UPROPERTY(EditAnywhere, Category = "Projectile | Setting")
	bool bIsPlayersProjectile = false;

	UPROPERTY(EditAnywhere, Category = "Projectile | Camera Shake")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditAnywhere, Category = "Projectile | Camera Shake")
	float CameraShakeRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Projectile | Setting", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	float Damage = 10.f;

	float HeadShotDamage = Damage * 1.5f;

	/** 폭발 범위 */
	UPROPERTY(EditAnywhere, Category = "Projectile | Setting", meta = (EditCondition = "bIsExplosive", ClampMin = "1.0", ClampMax = "1000.0"))
	float ExplosionRadius = 30.f;

	/** 충돌을 무시할 액터 */
	TArray<AActor*> IgnoreActors;

	/** 포스 작용 범위 */
	UPROPERTY(EditAnywhere)
	URadialForceComponent* RadialForce;

};
