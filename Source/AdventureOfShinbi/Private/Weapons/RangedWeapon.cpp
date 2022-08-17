// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RangedWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ARangedWeapon::ARangedWeapon()
{

}

void ARangedWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ARangedWeapon::Firing()
{
	PlayFireEffect();

	FHitResult HitResult;

	FVector2D ViewPortSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
	}

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
		FVector TraceStart = CrosshairWorldPosition;
		FVector TraceEnd = TraceStart + CrosshairWorldDirection * 10000.f;

		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

		// 적중하지 않았을 경우 충격 지점을 TraceEnd 로 지정
		if (!HitResult.bBlockingHit)
		{
			TraceHitEndPoint = TraceEnd;
		}
		else
		{
			TraceHitEndPoint = HitResult.ImpactPoint;
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
