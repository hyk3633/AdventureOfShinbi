// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RangedWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"

ARangedWeapon::ARangedWeapon()
{

}

void ARangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
	}
}

void ARangedWeapon::CrosshairLineTrace(FVector& OutHitPoint)
{
	FVector2D CrosshairLocation(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FHitResult HitResult;
		FVector TraceStart = CrosshairWorldPosition;
		FVector TraceEnd = TraceStart + CrosshairWorldDirection * 10000.f;

		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

		// 적중하지 않았을 경우 타격 지점을 TraceEnd 로 지정
		if (!HitResult.bBlockingHit)
		{
			OutHitPoint = TraceEnd;
		}
		else
		{
			OutHitPoint = HitResult.ImpactPoint;
		}
	}
}

void ARangedWeapon::PlayFireEffect()
{

	if (MuzzleFlashParticle)
	{
		const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleSocket");
		if (MuzzleSocket)
		{
			const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlashParticle, SocketTransform);
		}
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void ARangedWeapon::ConsumeAmmo()
{
	LoadedAmmo = FMath::Clamp(LoadedAmmo - AmmoConsumption, 0, Magazine);
}

ERangedWeaponType ARangedWeapon::GetRangedWeaponType() const
{
	return RangedWeaponType;
}

EAmmoType ARangedWeapon::GetAmmoType() const
{
	return AmmoType;
}

int32 ARangedWeapon::GetMagazine() const
{
	return Magazine;
}

int32 ARangedWeapon::GetLoadedAmmo() const
{
	return LoadedAmmo;
}

void ARangedWeapon::SetLoadedAmmo(int32 Quantity)
{
	LoadedAmmo = Quantity;
}

float ARangedWeapon::GetGunRecoil() const
{
	return GunRecoil;
}

bool ARangedWeapon::GetAutomaticFire() const
{
	return bAutomaticFire;
}

float ARangedWeapon::GetFireRate() const
{
	return FireRate;
}

float ARangedWeapon::GetZoomScope() const
{
	return ZoomScope;
}
