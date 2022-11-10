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
	PlayFireEffect(MuzzleFlashParticle, FireSound);

	if (bScatter)
	{
		ScatterFiring();
	}
	else
	{
		SingleFiring();
	}

	ConsumeAmmo();
}

void ARangedHitScanWeapon::ScatterFiring()
{
	FVector TraceStart;
	GetTraceStart(TraceStart);

	TArray<FVector> TraceEnd;
	for (int8 i = 0; i < NumberOfShots; i++)
	{
		TraceEnd.Add(GetTraceEnd(TraceStart));

		FHitResult TraceHitResult;
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, TraceStart, TraceEnd[i], ECollisionChannel::ECC_Visibility);

		DrawTrailParticle(TraceStart, TraceHitResult.ImpactPoint, TraceHitResult.ImpactNormal);
		ProcessHitResult(TraceHitResult);
	}
}

void ARangedHitScanWeapon::SingleFiring()
{
	FVector TraceStart;
	GetTraceStart(TraceStart);

	FVector TraceEnd = GetTraceEnd(TraceStart);

	FHitResult TraceHitResult;
	GetWorld()->LineTraceSingleByChannel(TraceHitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

	DrawTrailParticle(TraceStart, TraceHitResult.ImpactPoint, TraceHitResult.ImpactNormal);
	ProcessHitResult(TraceHitResult);
}

void ARangedHitScanWeapon::GetTraceStart(FVector& Start)
{
	CrosshairLineTrace(HitPoint);

	const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetItemMesh());

	Start = SocketTransform.GetLocation();
}

FVector ARangedHitScanWeapon::GetTraceEnd(const FVector& Start)
{
	if (BulletSpread > 0.f)
	{
		const FVector ToTarget = HitPoint - Start;
		const FVector RandomUnitVector = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ToTarget, BulletSpread);
		return Start + (RandomUnitVector * 5000.f);
	}
	else
	{
		return Start + (HitPoint - Start) * 1.25f;
	}
}

void ARangedHitScanWeapon::DrawTrailParticle(const FVector StartPoint, const FVector EndPoint, const FVector EndNormal)
{
	if (TrailParticle)
	{
		UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailParticle, StartPoint, EndNormal.Rotation());
		if (Trail)
		{
			FVector Direction = StartPoint - EndPoint;
			Direction.Normalize();
			Trail->SetVectorParameter(FName("Target"), Direction);
		}
	}
}

void ARangedHitScanWeapon::ProcessHitResult(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		ACharacter* DamagedActor = Cast<ACharacter>(HitResult.Actor);
		if (DamagedActor)
		{
			float Dmg = HitResult.BoneName == FName("head") ? GetHeadShotDamage() : GetWeaponDamage();
			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			UGameplayStatics::ApplyPointDamage(DamagedActor,Dmg,HitResult.ImpactNormal,HitResult,OwnerPawn->GetInstigatorController(),OwnerPawn,UDamageType::StaticClass());
		
			if (TargetHitParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, TargetHitParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
		}
		else
		{
			if (WorldHitParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, WorldHitParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
		}

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitResult.ImpactPoint);
		}
	}
}
