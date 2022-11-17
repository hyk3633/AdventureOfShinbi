
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

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	UPROPERTY(EditAnywhere, Category = "Projectile")
	bool bIsExplosive = false;

	UPROPERTY(EditAnywhere)
	bool bIsPlayersProjectile = false;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	float HeadShotDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (EditCondition = "bIsExplosive", ClampMin = "1.0", ClampMax = "1000.0"))
	float ExplosionRadius = 30.f;

	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (EditCondition = "bIsExplosive"))
	URadialForceComponent* RadialForce;

};
