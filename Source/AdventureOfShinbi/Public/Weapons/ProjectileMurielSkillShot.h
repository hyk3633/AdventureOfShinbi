
#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileHoming.h"
#include "ProjectileMurielSkillShot.generated.h"

/**
 * 중간 보스 뮤리엘 스킬 투사체 
 */
UCLASS()
class ADVENTUREOFSHINBI_API AProjectileMurielSkillShot : public AProjectileHoming
{
	GENERATED_BODY()
	
public:

	AProjectileMurielSkillShot();

protected:

	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:

};
