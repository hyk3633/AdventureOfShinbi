// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RangedHitScanWeapon.h"
#include "Weapons/Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy/EnemyCharacter.h"
#include "DrawDebugHelpers.h"

ARangedHitScanWeapon::ARangedHitScanWeapon()
{
	AmmoType = EAmmoType::EAT_AR;

	WeaponType = EWeaponType::EWT_AK47;

	Damage = 40.f;
}

void ARangedHitScanWeapon::Firing()
{
	ARangedWeapon::Firing();
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

	SpawnCasing();
}

void ARangedHitScanWeapon::SpawnCasing()
{
	if (CasingClass == nullptr)
		return;

	const USkeletalMeshSocket* EjectSocket = ItemMesh->GetSocketByName(FName("CasingEjectSocket"));
	if (EjectSocket == nullptr)
		return;
	const FTransform SocketTransform = EjectSocket->GetSocketTransform(ItemMesh);

	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		GetWorld()->SpawnActor<ACasing>(CasingClass, SocketTransform, SpawnParams);
	}
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

		DrawTrailParticle(TraceStart, TraceHitResult.ImpactPoint);
		ProcessHitResult(TraceHitResult);
	}
}

void ARangedHitScanWeapon::SingleFiring()
{
	// 메쉬의 총구 소켓 위치를 트레이스 시작 위치로 설정
	FVector TraceStart;
	GetTraceStart(TraceStart);

	// 크로스헤어 위치에서 수행한 라인 트레이스의 적중 위치와 시작 위치를 합한 값을 트레이스 끝 위치로 설정 
	FVector TraceEnd = GetTraceEnd(TraceStart);

	// 총구에서 크로스헤어가 조준하는 방향으로 라인 트레이스 수행
	FHitResult TraceHitResult;
	GetWorld()->LineTraceSingleByChannel(TraceHitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

	if (TraceHitResult.bBlockingHit == false)
	{
		TraceHitResult.ImpactPoint = TraceEnd;
	}

	DrawTrailParticle(TraceStart, TraceHitResult.ImpactPoint);
	ProcessHitResult(TraceHitResult);
}

void ARangedHitScanWeapon::GetTraceStart(FVector& Start)
{
	const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetItemMesh());

	Start = SocketTransform.GetLocation();
}

FVector ARangedHitScanWeapon::GetTraceEnd(const FVector& Start)
{
	CrosshairLineTrace(HitPoint);

	// BulletSpread 0 이상이면 타겟으로 향하는 벡터값을 랜덤하게 변환
	if (BulletSpread > 0.f)
	{
		const FVector ToTarget = HitPoint - Start;
		const FVector RandomUnitVector = 
			UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ToTarget, BulletSpread);
		return Start + (RandomUnitVector * 5000.f);
	}
	else
	{
		return Start + (HitPoint - Start) * 1.25f;
	}
}

void ARangedHitScanWeapon::DrawTrailParticle(const FVector StartPoint, const FVector EndPoint)
{
	if (TrailParticle)
	{
		UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailParticle, StartPoint, FRotator::ZeroRotator);
		if (Trail)
		{
			Trail->SetVectorParameter(FName("Target"), EndPoint);
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
			float Dmg = HitResult.BoneName == FName("head") ? Damage * 1.5f : Damage;
			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			UGameplayStatics::ApplyPointDamage
			(
				DamagedActor,
				Dmg,HitResult.
				ImpactNormal,
				HitResult,
				OwnerPawn->GetInstigatorController(),
				OwnerPawn,UDamageType::StaticClass()
			);
		
			if (TargetHitParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, TargetHitParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
			if (TargetHitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, TargetHitSound, HitResult.ImpactPoint);
			}
		}
		else
		{
			if (WorldHitParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, WorldHitParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
			if (WorldHitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, WorldHitSound, HitResult.ImpactPoint);
			}
		}
	}
}
