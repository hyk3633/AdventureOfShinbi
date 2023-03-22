// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileHoming.h"
#include "Enemy/EnemyRanged.h"
#include "Enemy/EnemyAIController.h"
#include "Player/AOSCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Sound/SoundCue.h"

AProjectileHoming::AProjectileHoming()
{
	ProjectileMovementComponent->bIsHomingProjectile = true;

	BodyParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BodyParticle"));
	BodyParticleComp->SetupAttachment(RootComponent);

	BodyNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BodyNiagara"));
	BodyNiagaraComp->SetupAttachment(RootComponent);

	bIsExplosive = true;
}

void AProjectileHoming::BeginPlay()
{
	Super::BeginPlay();

	if (bIsPlayersProjectile)
	{
		GetWorldTimerManager().SetTimer(CheckEnemyTimer, this, &AProjectileHoming::CheckNearbyEnemy, CheckEnemyTime, true);
	}
	else
	{
		// �� ������ ����ü�� ��� �÷��̾ ���� ������� ����
		AEnemyRanged* ER = Cast<AEnemyRanged>(GetInstigator());
		if (ER)
		{
			AAOSCharacter* Cha = Cast<AAOSCharacter>(ER->GetAiInfo().TargetPlayer);
			if (Cha)
			{
				ProjectileMovementComponent->HomingTargetComponent = Cha->GetCapsuleComponent();
			}
		}
	}

	GetWorldTimerManager().SetTimer(NoHitTimer, this, &AProjectileHoming::PlayNoHitParticle, LifeSpan - 2.f);

	//SetLifeSpan(LifeSpan);
}

void AProjectileHoming::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	BodyParticleComp->Deactivate();

	PlayHitEffect(Hit, OtherActor);

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AProjectileHoming::DestroyProjectile, DestroyTime);
}

void AProjectileHoming::PlayHitEffect(const FHitResult& Hit, AActor* OtherActor)
{
	if (TargetHitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TargetHitParticle, Hit.ImpactPoint, FRotator::ZeroRotator, false);
	}

	AEnemyCharacter* HittedEnemy = Cast<AEnemyCharacter>(OtherActor);
	AAOSCharacter* HittedPlayer = Cast<AAOSCharacter>(OtherActor);

	if (HittedEnemy)
	{
		HittedEnemy->PlayHitEffect(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}
	else if (HittedPlayer)
	{
		
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

void AProjectileHoming::PlayNoHitParticle()
{
	BodyParticleComp->Deactivate();
	BodyNiagaraComp->Deactivate();

	if (NoHitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NoHitParticle, GetActorLocation(), GetActorRotation(), false);
	}
}

void AProjectileHoming::DestroyProjectile()
{
	Destroy();
}

void AProjectileHoming::CheckNearbyEnemy()
{
	// �ݰ� 3000.f �̳��� �� ���͸� Ž��
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyCharacter::StaticClass(), Enemies);
	if (Enemies.Num() > 0)
	{
		float Dist = 3000.f;
		AActor* Target = nullptr;
		for (AActor* Enemy : Enemies)
		{
			const float DistToEnemy = GetDistanceTo(Enemy);
			if (DistToEnemy < Dist)
			{
				Dist = DistToEnemy;
				Target = Enemy;
			}
		}

		if (Target)
		{
			// ���� ��ȿ�ϸ� ���� ������� ����
			GetWorldTimerManager().ClearTimer(CheckEnemyTimer);
			AEnemyCharacter* EC = Cast<AEnemyCharacter>(Target);
			ProjectileMovementComponent->HomingTargetComponent = EC->GetCapsuleComponent();
		}
	}
}
