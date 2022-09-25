
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

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute")
	bool bIsExplosive = false;

	UPROPERTY(EditAnywhere)
	bool bIsPlayersProjectile = false;

protected:

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	float Damage = 5.f;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute", meta = (EditCondition = "bIsExplosive", ClampMin = "1.0", ClampMax = "1000.0"))
	float HeadShotDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute", meta = (EditCondition = "bIsExplosive", ClampMin = "1.0", ClampMax = "1000.0"))
	float ExplosionRadius = 30.f;

	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute", meta = (EditCondition = "bIsExplosive"))
	URadialForceComponent* RadialForce;
};
