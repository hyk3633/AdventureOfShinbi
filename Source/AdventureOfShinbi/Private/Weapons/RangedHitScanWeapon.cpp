// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RangedHitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

void ARangedHitScanWeapon::Firing()
{
	Super::Firing();

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());

	FHitResult MuzzleHitResult;
	FVector MuzzleTraceStart = SocketTransform.GetLocation();
	FVector MuzzleTraceEnd = TraceHitEndPoint;

	GetWorld()->LineTraceSingleByChannel(MuzzleHitResult, MuzzleTraceStart, MuzzleTraceEnd, ECollisionChannel::ECC_Visibility);

	if (MuzzleHitResult.bBlockingHit)
	{
		TraceHitEndPoint = MuzzleHitResult.Location;
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
			Trail->SetVectorParameter(FName("Target"), TraceHitEndPoint);
		}
	}
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticle, TraceHitEndPoint);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, TraceHitEndPoint);
	}
}
