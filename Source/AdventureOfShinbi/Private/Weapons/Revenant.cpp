// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Revenant.h"
#include "Player/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"

ARevenant::ARevenant()
{
	PrimaryActorTick.bCanEverTick = true;

	NumberOfShots = 6;
	ScatterRange = 10.f;
	Magazine = 6;

	bScatterGun = true;

	AmmoType = EAmmoType::EAT_Shell;

	WeaponType = EWeaponType::EWT_Revenent;
}

void ARevenant::BeginPlay()
{
	Super::BeginPlay();

	InitPartsLoc();

	if (ChargingParticle)
	{
		const USkeletalMeshSocket* ChamberSocket = ItemMesh->GetSocketByName(FName("ChamberSocket"));
		if (ChamberSocket == nullptr) return;
		const FTransform SocketTransform = ChamberSocket->GetSocketTransform(ItemMesh);

		ChargingParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			ChargingParticle,
			ItemMesh,
			NAME_None,
			SocketTransform.GetLocation(),
			SocketTransform.GetRotation().Rotator(),
			EAttachLocation::KeepWorldPosition,
			false,
			EPSCPoolMethod::None,
			false
		);
	}
}

void ARevenant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Charging(DeltaTime);
}

void ARevenant::Charging(float DeltaTime)
{
	// 우클릭이 눌려있고 장전된 탄약이 있으면 메쉬의 본 위치 값 업데이트
	if (bRightButtonClicking && GetLoadedAmmo() > 0)
	{
		if (CurrentPartsLoc.BarrelPitch >= 360.f)
		{
			CurrentPartsLoc.BarrelPitch = 0.f;
		}
		CurrentPartsLoc.BarrelPitch += DeltaLoc.BarrelPitch * DeltaTime;

		CurrentPartsLoc.BBL += DeltaLoc.BBL * DeltaTime;
		if (CurrentPartsLoc.BBL <= -9.f || (DeltaLoc.BBL > 0 && CurrentPartsLoc.BBL >= -8.f))
			DeltaLoc.BBL *= -1.f;
		CurrentPartsLoc.WBL += DeltaLoc.WBL * DeltaTime;
		if (CurrentPartsLoc.WBL <= -9.f || (DeltaLoc.WBL > 0 && CurrentPartsLoc.WBL >= -8.f))
			DeltaLoc.WBL *= -1.f;

		CurrentPartsLoc.BBR += DeltaLoc.BBR * DeltaTime; // Y 오른쪽으로
		if (CurrentPartsLoc.BBR <= -9.f || (DeltaLoc.BBR > 0 && CurrentPartsLoc.BBR >= -8.f))
			DeltaLoc.BBR *= -1.f;
		CurrentPartsLoc.WBR += DeltaLoc.WBR * DeltaTime;
		if (CurrentPartsLoc.WBR <= -9.f || (DeltaLoc.WBR > 0 && CurrentPartsLoc.WBR >= -8.f))
			DeltaLoc.WBR *= -1.f;

		CurrentPartsLoc.BTL += DeltaLoc.BTL * DeltaTime; // 왼쪽
		if (CurrentPartsLoc.BTL >= 9.f || (DeltaLoc.BTL < 0 && CurrentPartsLoc.BTL <= 8.f))
			DeltaLoc.BTL *= -1.f;
		CurrentPartsLoc.WTL += DeltaLoc.WTL * DeltaTime;
		if (CurrentPartsLoc.WTL >= 9.f || (DeltaLoc.WTL < 0 && CurrentPartsLoc.WTL <= 8.f))
			DeltaLoc.WTL *= -1.f;

		CurrentPartsLoc.BTR += DeltaLoc.BTR * DeltaTime; // 위쪽
		if (CurrentPartsLoc.BTR >= 9.f || (DeltaLoc.BTR < 0 && CurrentPartsLoc.BTR <= 8.f))
			DeltaLoc.BTR *= -1.f;
		CurrentPartsLoc.WTR += DeltaLoc.WTR * DeltaTime;
		if (CurrentPartsLoc.WTR >= 9.f || (DeltaLoc.WTR < 0 && CurrentPartsLoc.WTR <= 8.f))
			DeltaLoc.WTR *= -1.f;

		// 우클릭이 2초이상 유지되면 파티클 활성화
		ChargingTime += DeltaTime;
		if (ChargingTime >= 2.f && ChargingParticleComp->IsActive() == false)
		{
			ChargingParticleComp->Activate();
		}
	}
	else
	{
		InitPartsLoc();

		ChargingTime = 0.f;
		ChargingParticleComp->Deactivate();
	}
}

void ARevenant::Firing()
{
	if (bRightButtonClicking && ChargingTime>= 2.f)
	{
		ChargeShot();
		ConsumeAmmo();

		ScatterRange = 10.f;
		AmmoConsumption = 1;
	}
	else
	{
		Super::Firing();

		PlayFireEffect(MuzzleFlash2, nullptr);

		SingleFiring(ObliterateClass);
	}

	WeaponOwner->DeactivateAiming();

	bRightButtonClicking = false;
}

void ARevenant::ChargeShot()
{
	ScatterRange = 5.f;
	AmmoConsumption = Magazine;

	PlayFireEffect(ChargeShotMuzzleFlash, ChargeShotSound);
	PlayFireEffect(ChargeShotMuzzleFlash2, nullptr);

	ScatterFiring(ChargeShotPorjClass);
}

void ARevenant::RightButtonClicking(bool bClicking)
{
	bRightButtonClicking = bClicking;
	
	if (bClicking)
	{
		if (ChargingLoopSound)
		{
			ChargingLoopSoundComp = UGameplayStatics::SpawnSound2D(this, ChargingLoopSound);
		}
	}
	else
	{
		if (ChargingLoopSoundComp)
		{
			ChargingLoopSoundComp->DestroyComponent();
		}
	}
}

void ARevenant::SetWeaponState(const EWeaponState State)
{
	Super::SetWeaponState(State);

	if (GetWeaponState() == EWeaponState::EWS_Equipped && bDelBinded == false)
	{
		WeaponOwner = Cast<AAOSCharacter>(GetOwner());
		if (WeaponOwner)
		{
			WeaponOwner->DAimButtonPressed.BindUObject(this, &ARevenant::RightButtonClicking);
			bDelBinded = true;
		}

		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetEnableGravity(true);
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

void ARevenant::InitPartsLoc()
{
	CurrentPartsLoc.BarrelPitch = 0.f;

	CurrentPartsLoc.BBL = 0.f;
	CurrentPartsLoc.WBL = 0.f;
	CurrentPartsLoc.BBR = 0.f;
	CurrentPartsLoc.WBR = 0.f;
	CurrentPartsLoc.BTL = 0.f;
	CurrentPartsLoc.WTL = 0.f;
	CurrentPartsLoc.BTR = 0.f;
	CurrentPartsLoc.WTR = 0.f;

	DeltaLoc.BarrelPitch = 1000.f;

	DeltaLoc.BBL = -10.f;
	DeltaLoc.WBL = -9.5f;
	DeltaLoc.BBR = -9.5f;
	DeltaLoc.WBR = -10.f;
	DeltaLoc.BTL = 10.f;
	DeltaLoc.WTL = 9.5f;
	DeltaLoc.BTR = 9.5f;
	DeltaLoc.WTR = 10.f;

}
