
#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectile.h"
#include "ProjectileShinbiWolf.generated.h"

/**
 * 
 */

class USkeletalMeshComponent;
class UAnimationAsset;
class UAnimMontage;
class UParticleSystem;
class USoundCue;

UENUM()
enum class EShinbiWolfMode : uint8 {
	ESW_WolfAttack,
	ESW_CirclingWolves,
	ESW_UltimateWolf,

	ESW_MAX
};

UCLASS()
class ADVENTUREOFSHINBI_API AProjectileShinbiWolf : public AProjectile
{
	GENERATED_BODY()
	
public:

	AProjectileShinbiWolf();

	void WolfAttackMode();

	void CirclingWolvesMode();

	void CirclingWolvesEnd();

	void UltimateWolfRushMode();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void InitHittedEnemy();

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	void WolfJump();

	void LifeOver();

	void DestoryProj();

private:

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* WolfMesh;

	EShinbiWolfMode WolfMode = EShinbiWolfMode::ESW_WolfAttack;

	FTimerHandle LifeTimer;
	float LifeTime = 0.77f;

	FTimerHandle DestroyDelayTimer;
	float DestroyDelayTime = 1.0f;

	UPROPERTY()
	AActor* HittedEnemy;

	FTimerHandle InitHittedEnemyTimer;

	/** Wolf Attack */

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Wolf Attack")
	UAnimMontage* WolfAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Wolf Attack")
	UParticleSystem* WolfAttackImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Wolf Attack")
	UParticleSystem* WolfAttackNoHitParticle;

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Wolf Attack")
	UParticleSystem* WolfAttackTrail;

	UPROPERTY()
	UParticleSystemComponent* WolfAttackTrailComp;

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Wolf Attack")
	USoundCue* NormalImpactSound;

	bool bHitted = false;

	/** Circling Wolves */

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Circling Wolves")
	UAnimationAsset* CirclingWolvesAnim;

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Circling Wolves")
	UParticleSystem* CirclingWolvesTrail;

	UPROPERTY()
	UParticleSystemComponent* CirclingWolvesTrailComp;

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Circling Wolves")
	UParticleSystem* CirclingWolvesImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Circling Wolves")
	UParticleSystem* WolfRemovalParticle;

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Circling Wolves")
	USoundCue* SmallImpactSound;

	/** Ultimate Wolf */

	UPROPERTY(EditAnywhere, Category = "Shinbi Wolf | Ultimate Wolf")
	UAnimationAsset* UltimateWolfRushAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	UParticleSystem* UltimateWolfEndParticle;

public:



};
