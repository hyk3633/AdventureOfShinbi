
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class URadialForceComponent;

/**
* ����ü �⺻ Ŭ���� 
*/

DECLARE_DELEGATE(DeactivatePooledObject)

UCLASS()
class ADVENTUREOFSHINBI_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AProjectile();

	/** �߻��ϴ� ��ü���� ������ ���� */
	void SetDamage(float Value);

	FORCEINLINE virtual void SetLifeSpan(float Life) override { LifeSpan = Life; };

	virtual void Activate();

	virtual void Deactivate();

	FORCEINLINE bool GetIsActive() const { return bIsActive; }

	DeactivatePooledObject DDeactivatePooledObject;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	/** �浹 ���� ������Ʈ */
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision;

	/** ����ü�� �������� ����ϴ� ������Ʈ */
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float LifeSpan = 1.f;

	FTimerHandle LifeTimer;

	/** true �� ��� RadialDamage�� ���� */
	UPROPERTY(EditAnywhere, Category = "Projectile | Setting")
	bool bIsExplosive = false;

	/** true �� ��� �÷��̾ �����ϴ� ����ü */
	UPROPERTY(EditAnywhere, Category = "Projectile | Setting")
	bool bIsPlayersProjectile = false;

	UPROPERTY(EditAnywhere, Category = "Projectile | Camera Shake")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditAnywhere, Category = "Projectile | Camera Shake")
	float CameraShakeRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Projectile | Setting", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	float Damage = 10.f;

	float HeadShotDamage = Damage * 1.5f;

	/** ���� ���� */
	UPROPERTY(EditAnywhere, Category = "Projectile | Setting", meta = (EditCondition = "bIsExplosive", ClampMin = "1.0", ClampMax = "1000.0"))
	float ExplosionRadius = 30.f;

	/** �浹�� ������ ���� */
	UPROPERTY()
	TArray<AActor*> IgnoreActors;

	/** ���� �ۿ� ���� */
	UPROPERTY(EditAnywhere)
	URadialForceComponent* RadialForce;

private:

	bool bIsActive;

};
