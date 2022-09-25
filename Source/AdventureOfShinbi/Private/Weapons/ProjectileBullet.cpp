

#include "Weapons/ProjectileBullet.h"
#include "Player/AOSCharacter.h"
#include "Enemy/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	if (BulletParticle)
	{
		BulletComponent = UGameplayStatics::SpawnEmitterAttached(
			BulletParticle,
			BoxCollision,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	SetLifeSpan(LifeSpan);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	AEnemyCharacter* HittedEnemy = Cast<AEnemyCharacter>(OtherActor);
	AAOSCharacter* HittedPlayer = Cast<AAOSCharacter>(OtherActor);

	if (HittedEnemy)
	{
		HittedEnemy->PlayHitEffect(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}
	else if (HittedPlayer)
	{
		// TODO �÷��̾� ���� ��Ʈ ����Ʈ
	}
	else
	{
		// ĳ���͵� ���� �ƴ� ��ü�� ������ ��
		if (HitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), false);
		}
	}

	Destroy();
}
