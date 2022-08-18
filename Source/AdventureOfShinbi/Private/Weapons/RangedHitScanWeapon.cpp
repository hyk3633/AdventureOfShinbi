// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RangedHitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

void ARangedHitScanWeapon::Firing()
{
	PlayFireEffect();

	CrosshairLineTrace(HitPoint);

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());

	FHitResult MuzzleHitResult;
	FVector MuzzleTraceStart = SocketTransform.GetLocation();
	FVector MuzzleTraceEnd = MuzzleTraceStart + (HitPoint - MuzzleTraceStart) * 1.25f;

	GetWorld()->LineTraceSingleByChannel(MuzzleHitResult, MuzzleTraceStart, MuzzleTraceEnd, ECollisionChannel::ECC_Visibility);

	if (MuzzleHitResult.bBlockingHit)
	{
		HitPoint = MuzzleHitResult.ImpactPoint;
		ImpactRotator = MuzzleHitResult.ImpactNormal.Rotation();
		// 데미지 적용
		float Dmg = MuzzleHitResult.BoneName == FName("head") ? HeadShotDamage : Damage;
		AActor* DamagedActor = Cast<AActor>(MuzzleHitResult.Actor);
		APawn* OwnerPawn = Cast<APawn>(GetOwner());

		UGameplayStatics::ApplyDamage(DamagedActor, Dmg, OwnerPawn->GetController(), OwnerPawn, UDamageType::StaticClass());
	}

	PlayAfterFireEffect();
}

void ARangedHitScanWeapon::PlayAfterFireEffect()
{
	if (TrailParticle)
	{
		const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleSocket");
		if (MuzzleSocket == nullptr) return;
		const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());

		UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailParticle, SocketTransform);
		if (Trail)
		{
			Trail->SetVectorParameter(FName("Target"), HitPoint);
		}
	}
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticle, HitPoint, ImpactRotator);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitPoint);
	}
}
