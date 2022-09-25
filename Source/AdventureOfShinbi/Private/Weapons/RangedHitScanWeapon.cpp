// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RangedHitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy/EnemyCharacter.h"
#include "DrawDebugHelpers.h"

void ARangedHitScanWeapon::Firing()
{
	PlayFireEffect();

	CrosshairLineTrace(HitPoint);

	const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetItemMesh());

	FHitResult MuzzleHitResult;
	FVector MuzzleTraceStart = SocketTransform.GetLocation();
	FVector MuzzleTraceEnd = MuzzleTraceStart + (HitPoint - MuzzleTraceStart) * 1.25f;

	if (BulletSpread > 0.f)
	{
		FVector ToTarget = HitPoint - SocketTransform.GetLocation();
		FVector RandomUnitVector = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ToTarget, BulletSpread);
		MuzzleTraceEnd = MuzzleTraceStart + (RandomUnitVector * 5000.f);
	}

	GetWorld()->LineTraceSingleByChannel(MuzzleHitResult, MuzzleTraceStart, MuzzleTraceEnd, ECollisionChannel::ECC_Visibility);

	if (MuzzleHitResult.bBlockingHit)
	{
		HitPoint = MuzzleHitResult.ImpactPoint;
		ImpactRotator = MuzzleHitResult.ImpactNormal.Rotation();

		AEnemyCharacter* DamagedActor = Cast<AEnemyCharacter>(MuzzleHitResult.Actor);
		if (DamagedActor)
		{
			float Dmg = MuzzleHitResult.BoneName == FName("head") ? HeadShotDamage : Damage;
			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			UGameplayStatics::ApplyPointDamage(DamagedActor, Dmg, GetActorLocation(), MuzzleHitResult, OwnerPawn->GetInstigatorController(), OwnerPawn, UDamageType::StaticClass());
			DamagedActor->PlayHitEffect(HitPoint, ImpactRotator);
		}
		else
		{
			if (ImpactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticle, HitPoint, ImpactRotator);
			}
		}
	}

	PlayAfterFireEffect();
}

void ARangedHitScanWeapon::PlayAfterFireEffect()
{
	if (TrailParticle)
	{
		const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName("MuzzleSocket");
		if (MuzzleSocket == nullptr) return;
		const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetItemMesh());

		UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailParticle, SocketTransform);
		if (Trail)
		{
			Trail->SetVectorParameter(FName("Target"), HitPoint);
		}
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitPoint);
	}
}
