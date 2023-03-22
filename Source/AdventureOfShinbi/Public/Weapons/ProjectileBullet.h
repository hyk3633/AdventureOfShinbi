
#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 유도 기능이 없는 일반적인 투사체 클래스
 */

class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()
	
public:
	
	AProjectileBullet();

	virtual void Activate() override;

	virtual void Deactivate() override;

protected:

	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void PlayHitEffect(const FHitResult& Hit, AActor* OtherActor);

	void PlayNoHitParticle();

protected:

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* BodyParticleComponent;

private:

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* BulletParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* TargetHitParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* WorldHitParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* NoHitParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	USoundCue* HitSound;

	FTimerHandle NoHitTimer;

};
