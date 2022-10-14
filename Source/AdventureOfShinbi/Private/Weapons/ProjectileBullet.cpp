

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

	GetWorldTimerManager().SetTimer(NoHitTimer, this, &AProjectileBullet::PlayNoHitParticle, LifeSpan - 2.f);

	SetLifeSpan(LifeSpan);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

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
		// TODO 플레이어 전용 히트 이펙트
	}
	else
	{
		// 캐릭터도 적도 아닌 물체를 맞혔을 때
		if (WorldHitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WorldHitParticle, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), false);
		}
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}

	Destroy();
}

void AProjectileBullet::PlayNoHitParticle()
{
	BodyParticleComponent->Deactivate();

	if (NoHitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NoHitParticle, GetActorLocation(), GetActorRotation(), false);
	}
}
