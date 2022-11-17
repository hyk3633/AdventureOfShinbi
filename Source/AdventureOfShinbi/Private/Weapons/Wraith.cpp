// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Wraith.h"
#include "Player/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"

AWraith::AWraith()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWraith::BeginPlay()
{
	Super::BeginPlay();

	InitializingWeaponPartsLocation();
}

void AWraith::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InterpWeaponPartsLocation(DeltaTime, bAimed);
}

void AWraith::InterpWeaponPartsLocation(float DeltaTime, bool bSniperMode)
{
	if (bSniperMode)
	{
		CurrentValue.Barrel = FMath::VInterpTo(CurrentValue.Barrel, SniperMode.Barrel, DeltaTime, FormChangeSpeed);
		CurrentValue.Scope = FMath::VInterpTo(CurrentValue.Scope, SniperMode.Scope, DeltaTime, FormChangeSpeed);
		CurrentValue.Lens = FMath::VInterpTo(CurrentValue.Lens, SniperMode.Lens, DeltaTime, FormChangeSpeed);
		CurrentValue.StockBott = FMath::VInterpTo(CurrentValue.StockBott, SniperMode.StockBott, DeltaTime, FormChangeSpeed);
		CurrentValue.StockBottRot = FMath::RInterpTo(CurrentValue.StockBottRot, SniperMode.StockBottRot, DeltaTime, FormChangeSpeed);
		CurrentValue.StockTop = FMath::VInterpTo(CurrentValue.StockTop, SniperMode.StockTop, DeltaTime, FormChangeSpeed);
	}
	else
	{
		CurrentValue.Barrel = FMath::VInterpTo(CurrentValue.Barrel, AssaultMode.Barrel, DeltaTime, FormChangeSpeed);
		CurrentValue.Scope = FMath::VInterpTo(CurrentValue.Scope, AssaultMode.Scope, DeltaTime, FormChangeSpeed);
		CurrentValue.Lens = FMath::VInterpTo(CurrentValue.Lens, AssaultMode.Lens, DeltaTime, FormChangeSpeed);
		CurrentValue.StockBott = FMath::VInterpTo(CurrentValue.StockBott, AssaultMode.StockBott, DeltaTime, FormChangeSpeed);
		CurrentValue.StockBottRot = FMath::RInterpTo(CurrentValue.StockBottRot, AssaultMode.StockBottRot, DeltaTime, FormChangeSpeed);
		CurrentValue.StockTop = FMath::VInterpTo(CurrentValue.StockTop, AssaultMode.StockTop, DeltaTime, FormChangeSpeed);
	}
}

void AWraith::Firing()
{
	if (bAimed)
	{
		PlayFireEffect(AimedMuzzleFlashParticle, AimedFireSound);
		PlayFireEffect(AimedSmokeParticle, nullptr);
		PlayFireEffect(AimedStabilizerParticle, nullptr);

		SingleFiring(AimingProjectileClass);

		ConsumeAmmo();
	}
	else
	{
		Super::Firing();
	}
}

void AWraith::SetWeaponState(const EWeaponState State)
{
	Super::SetWeaponState(State);

	if (GetWeaponState() == EWeaponState::EWS_Equipped && bDelBinded == false)
	{
		WeaponOwner = Cast<AAOSCharacter>(GetOwner());
		if (WeaponOwner)
		{
			WeaponOwner->DAimButtonPressed.BindUObject(this, &AWraith::FormChange);
			bDelBinded = true;
		}
	}
	else
	{
		if (bDelBinded)
		{
			WeaponOwner->DAimButtonPressed.Unbind();
			bDelBinded = false;
		}
	}
}

void AWraith::FormChange(bool bSniperMode)
{
	if (bSniperMode)
	{
		bAimed = true;
		bAutomaticFire = false;
		FireRate = 1.5f;
		GunRecoil = 0.6f;
		if (ActivateSound)
		{
			UGameplayStatics::PlaySound2D(this, ActivateSound);
		}
		if (WeaponOwner)
		{
			ActivateScopeParticle();
		}
	}
	else
	{
		bAimed = false;
		bAutomaticFire = true;
		FireRate = 0.1;
		GunRecoil = 0.35f;
		if (DeactivateSound)
		{
			UGameplayStatics::PlaySound2D(this, DeactivateSound);
		}

		if (ScopeParticleComp)
		{
			ScopeParticleComp->Deactivate();
		}
	}
}

void AWraith::ActivateScopeParticle()
{
	const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName("ScopeSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetItemMesh());

	if (ScopeParticle && ScopeParticleComp == nullptr)
	{
		ScopeParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			ScopeParticle,
			ItemMesh,
			NAME_None,
			SocketTransform.GetLocation(),
			SocketTransform.GetRotation().Rotator(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	else if (ScopeParticleComp)
	{
		ScopeParticleComp->Activate();
	}
}

void AWraith::InitializingWeaponPartsLocation()
{
	AssaultMode.Barrel = FVector(0.f, -32.3f, 0.f);
	AssaultMode.Scope = FVector(0.f,0.f,-4.f);
	AssaultMode.Lens = FVector::ZeroVector;
	AssaultMode.StockBott = FVector(0.f,5.f,0.f);
	AssaultMode.StockBottRot = FRotator::ZeroRotator;
	AssaultMode.StockTop = FVector(0.f, 5.f, 0.f);

	SniperMode.Barrel = FVector::ZeroVector;
	SniperMode.Scope = FVector::ZeroVector;
	SniperMode.Lens = FVector(0.f, -2.f, 0.f);
	SniperMode.StockBott = FVector(0.f,3.f,-3.f);
	SniperMode.StockBottRot = FRotator(0.f, 0.f, -20.f);
	SniperMode.StockTop = FVector::ZeroVector;

	CurrentValue = AssaultMode;
}

