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

	bool bEnemyHit = false;
	if (MuzzleHitResult.bBlockingHit)
	{
		HitPoint = MuzzleHitResult.ImpactPoint;
		ImpactRotator = MuzzleHitResult.ImpactNormal.Rotation();

		ACharacter* DamagedActor = Cast<ACharacter>(MuzzleHitResult.Actor);
		if (DamagedActor)
		{
			float Dmg = MuzzleHitResult.BoneName == FName("head") ? GetHeadShotDamage() : GetWeaponDamage();
			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			UGameplayStatics::ApplyPointDamage(DamagedActor, Dmg, GetActorLocation(), MuzzleHitResult, OwnerPawn->GetInstigatorController(), OwnerPawn, UDamageType::StaticClass());
			
			bEnemyHit = true;
			//DamagedActor->PlayHitEffect(HitPoint, ImpactRotator);
		}
	}

	PlayAfterFireEffect(bEnemyHit, MuzzleHitResult.bBlockingHit);
}

void ARangedHitScanWeapon::PlayAfterFireEffect(const bool EnemyHit, const bool HitAnything)
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
	if (EnemyHit)
	{
		if (TargetHitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, TargetHitParticle, HitPoint, ImpactRotator);
		}
	}
	else
	{
		if (WorldHitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, WorldHitParticle, HitPoint, ImpactRotator);
		}
	}
	if (HitSound && HitAnything)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitPoint);
	}
}
