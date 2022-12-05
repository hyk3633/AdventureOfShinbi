
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class URadialForceComponent;

UCLASS()
class ADVENTUREOFSHINBI_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AProjectile();

	void SetDamage(float Value);

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile | Setting")
	bool bIsExplosive = false;

	UPROPERTY(EditAnywhere, Category = "Projectile | Setting")
	bool bIsPlayersProjectile = false;

	UPROPERTY(EditAnywhere, Category = "Projectile | Camera Shake")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditAnywhere, Category = "Projectile | Camera Shake")
	float CameraShakeRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Projectile | Setting", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	float Damage = 10.f;

	float HeadShotDamage = Damage * 1.5f;

	UPROPERTY(EditAnywhere, Category = "Projectile | Setting", meta = (EditCondition = "bIsExplosive", ClampMin = "1.0", ClampMax = "1000.0"))
	float ExplosionRadius = 30.f;

	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditAnywhere)
	URadialForceComponent* RadialForce;

};
