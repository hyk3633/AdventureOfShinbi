

#include "Weapons/ProjectileBullet.h"
#include "Player/AOSCharacter.h"
#include "Enemy/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

AProjectileBullet::AProjectileBullet()
{
	BodyParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BodyParticle"));
	BodyParticleComponent->SetupAttachment(RootComponent);

}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	if (BulletParticle)
	{
		BodyParticleComponent = UGameplayStatics::SpawnEmitterAttached(
			BulletParticle,
			BoxCollision,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	GetWorldTimerManager().SetTimer(LifeTimer, this, &AProjectileBullet::LifeOver, LifeSpan);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	PlayHitEffect(Hit, OtherActor);

	LifeOver();
}

void AProjectileBullet::PlayHitEffect(const FHitResult& Hit, AActor* OtherActor)
{
	if (TargetHitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TargetHitParticle, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), false);
	}

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
		if (WorldHitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WorldHitParticle, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), false);
		}
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
}

void AProjectileBullet::LifeOver()
{
	BodyParticleComponent->Deactivate();

	if (NoHitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NoHitParticle, GetActorLocation(), GetActorRotation(), false);
	}

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AProjectileBullet::DestroyProjectile, 2.f);
}

void AProjectileBullet::DestroyProjectile()
{
	Destroy();
}
