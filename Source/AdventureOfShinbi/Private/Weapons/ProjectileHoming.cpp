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
#include "BehaviorTree/BlackboardComponent.h"
#include "Sound/SoundCue.h"

AProjectileHoming::AProjectileHoming()
{
	ProjectileMovementComponent->bIsHomingProjectile = true;

	BodyParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BodyParticle"));
	BodyParticleComponent->SetupAttachment(RootComponent);
	
	bIsExplosive = true;
}

void AProjectileHoming::BeginPlay()
{
	Super::BeginPlay();

	if (bIsPlayersProjectile)
	{
		// TODO : getalloverlapactors
	}
	else
	{
		AEnemyRanged* ER = Cast<AEnemyRanged>(GetInstigator());
		if (ER)
		{
			AAOSCharacter* Cha = Cast<AAOSCharacter>(ER->GetEnemyController()->GetBlackBoard()->GetValueAsObject(FName("Target")));
			if (Cha)
			{
				ProjectileMovementComponent->HomingTargetComponent = Cha->GetCapsuleComponent();
			}
		}
	}
}

void AProjectileHoming::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Hit.ImpactPoint, NormalImpulse.Rotation());
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	GetWorldTimerManager().SetTimer(ParticleOffTimer, this, &AProjectileHoming::ParticleOff, ParticleOffTime);
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AProjectileHoming::DestroyProjectile, DestroyTime);
}

void AProjectileHoming::DestroyProjectile()
{
	Destroy();
}

void AProjectileHoming::ParticleOff()
{
	BodyParticleComponent->SetVisibility(false);
}
