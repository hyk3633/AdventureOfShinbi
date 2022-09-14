// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileBullet.h"
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

	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), false);
	}

	Destroy();
}
