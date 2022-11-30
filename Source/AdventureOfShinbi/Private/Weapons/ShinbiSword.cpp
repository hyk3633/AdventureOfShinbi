// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ShinbiSword.h"
#include "Weapons/ProjectileShinbiWolf.h"
#include "Player/AOSCharacter.h"
#include "Enemy/EnemyCharacter.h"
#include "Components/CombatComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

AShinbiSword::AShinbiSword()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponType = EWeaponType::EWT_ShinbiSword;
	CapsuleRadius = 15.f;
	CapsuleHalfHeight = 10.f;
}

void AShinbiSword::BeginPlay()
{
	Super::BeginPlay();

	for (int8 i = 0; i < 5; i++)
	{
		AngleOffset.Add(-72.f * i);
	}

	if (SwordGlowLoop)
	{
		const USkeletalMeshSocket* GlowSocket = ItemMesh->GetSocketByName(FName("GlowSocket"));
		if (GlowSocket == nullptr)
			return;
		const FTransform SocketTransform = GlowSocket->GetSocketTransform(ItemMesh);

		SwordGlowLoopComp = UGameplayStatics::SpawnEmitterAttached
		(
			SwordGlowLoop,
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

void AShinbiSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CirclingWolvesOn(DeltaTime);
}

void AShinbiSword::SetWeaponState(const EWeaponState State)
{
	Super::SetWeaponState(State);

	if (GetWeaponState() == EWeaponState::EWS_Equipped)
	{
		WeaponOwner = Cast<AAOSCharacter>(GetOwner());

		if (SwordGlowStart)
		{
			const USkeletalMeshSocket* GlowSocket = ItemMesh->GetSocketByName(FName("GlowSocket"));
			if (GlowSocket == nullptr)
				return;
			const FTransform SocketTransform = GlowSocket->GetSocketTransform(ItemMesh);

			UGameplayStatics::SpawnEmitterAttached
			(
				SwordGlowStart,
				ItemMesh,
				NAME_None,
				SocketTransform.GetLocation(),
				SocketTransform.GetRotation().Rotator(),
				EAttachLocation::KeepWorldPosition
			);

			GetWorldTimerManager().SetTimer(GlowTimer, this, &AShinbiSword::ActivateGlow, 0.3f);
		}
	}
	else
	{
		if (SwordGlowLoopComp)
		{
			SwordGlowLoopComp->Deactivate();
		}
	}
}

void AShinbiSword::ActivateGlow()
{
	if (SwordGlowLoopComp)
	{
		SwordGlowLoopComp->Activate();
	}
}

void AShinbiSword::CirclingWolvesOn(float DeltaTime)
{
	if (bCirclingWolvesOn == false)
		return;

	if (CirclingWolvesArr.Num() > 0)
	{
		FVector Loc = GetOwner()->GetActorLocation();
		for (int8 i = 0; i < 5; i++)
		{
			FVector Dim = FVector(300.f, 0.f, 0.f);
			AngleAxis[i] += DeltaTime * Multiplier;

			if (AngleAxis[i] >= 360.0f)
			{
				AngleAxis[i] = 0;
			}

			FVector RotateValue = Dim.RotateAngleAxis(AngleAxis[i] + AngleOffset[i], FVector(0,0,1));

			FRotator Rotation = CirclingWolvesArr[i]->GetActorRotation();
			Rotation += FRotator(0.f, RotateSpeed, 0.f) * DeltaTime;

			CirclingWolvesArr[i]->SetActorLocation(Loc + RotateValue);
			CirclingWolvesArr[i]->SetActorRotation(Rotation);
		}
	}
}

void AShinbiSword::WolfAttack()
{
	if (WolfProjClass == nullptr)
		return;

	if (WeaponOwner == nullptr)
	{
		WeaponOwner = Cast<AAOSCharacter>(GetOwner());
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = WeaponOwner;
	SpawnParams.Instigator = WeaponOwner;

	FVector SpawnLocation = WeaponOwner->GetActorLocation();
	SpawnLocation = SpawnLocation + (WeaponOwner->GetActorForwardVector() * 100.f) - FVector(0.f,0.f,40.f);
	const FRotator SpawnRotation = WeaponOwner->GetActorRotation();

	AProjectile* SpawnedProj = GetWorld()->SpawnActor<AProjectile>(WolfProjClass, SpawnLocation, SpawnRotation, SpawnParams);
	WolfProj = Cast<AProjectileShinbiWolf>(SpawnedProj);
	WolfProj->WolfAttackMode();

	if (WolfAttackSound)
	{
		UGameplayStatics::PlaySound2D(this, WolfAttackSound);
	}
}

void AShinbiSword::CirclingWolves()
{
	if (WolfProjClass == nullptr)
		return;

	CirclingWolvesArr.Empty();
	
	if (WeaponOwner == nullptr)
	{
		WeaponOwner = Cast<AAOSCharacter>(GetOwner());
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = WeaponOwner;
	SpawnParams.Instigator = WeaponOwner;

	AngleAxis.Init(0.f, 5);

	PlayCirclingWolvesEffect();
	
	FVector Center = WeaponOwner->GetActorLocation();
	FRotator FirstRot = FRotator(0.f, -18.f, 0.f);

	for (int8 i = 0; i < 5; i++)
	{
		FVector RotateValue = FVector(300.f, 0.f, 0.f).RotateAngleAxis(-72.f * i, FVector(0,0,1));

		FirstRot += FRotator(0.f, -72.f, 0.f);

		AProjectile* SpawnedProj = GetWorld()->SpawnActor<AProjectile>(WolfProjClass, Center + RotateValue, FirstRot, SpawnParams);
		AProjectileShinbiWolf* WolfProjs = Cast<AProjectileShinbiWolf>(SpawnedProj);
		WolfProjs->CirclingWolvesMode();

		CirclingWolvesArr.Add(WolfProjs);
	}
	
	bCirclingWolvesOn = true;

	GetWorldTimerManager().SetTimer(CirclingWolvesDurationTimer, this, &AShinbiSword::CirclingWolvesDurationEnd, CirclingWolvesDurationTime);
}

void AShinbiSword::PlayCirclingWolvesEffect()
{
	if (CirclingParticle)
	{
		CirclingParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			CirclingParticle,
			WeaponOwner->GetMesh(),
			NAME_None,
			WeaponOwner->GetActorLocation(),
			FRotator::ZeroRotator,
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (LensFlareParticle)
	{
		LensFlareParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			LensFlareParticle,
			WeaponOwner->GetMesh(),
			NAME_None,
			WeaponOwner->GetActorLocation(),
			FRotator::ZeroRotator,
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (CirclingWolvesSound)
	{
		UGameplayStatics::PlaySound2D(this, CirclingWolvesSound);
	}
	if (CirclingLoopingSound)
	{
		CirclingLoopingSoundComp = UGameplayStatics::SpawnSound2D(this, CirclingLoopingSound, 1.f, 1.f, 0.f, nullptr, false, false);
	}
}

void AShinbiSword::CirclingWolvesDurationEnd()
{
	if (WeaponOwner == nullptr)
	{
		WeaponOwner = Cast<AAOSCharacter>(GetOwner());
	}
	WeaponOwner->GetCombatComp()->CirclingWolvesEnd();

	CirclingParticleComp->Deactivate();
	LensFlareParticleComp->Deactivate();
	CirclingLoopingSoundComp->Stop();

	for (int8 i = 0; i < 5; i++)
	{
		CirclingWolvesArr[i]->CirclingWolvesEnd();
	}

	bCirclingWolvesOn = false;
}

void AShinbiSword::UltimateWolfRush()
{
	Enemies.Empty();
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyCharacter::StaticClass(), Enemies);

	for (int8 i = 0; i < Enemies.Num(); i++)
	{
		FVector TowardsTarget = Enemies[i]->GetActorLocation() - WeaponOwner->GetActorLocation();
		TowardsTarget.Normalize();
		if (FVector::DotProduct(WeaponOwner->GetActorForwardVector(), TowardsTarget) < 0.4 || WeaponOwner->GetDistanceTo(Enemies[i]) > 800.f)
		{
			Enemies.RemoveAt(i);
			i -= 1;
		}
	}

	if (Enemies.Num() == 0)
	{
		return;
	}

	if (LightParticle)
	{
		LightParticleComp = UGameplayStatics::SpawnEmitterAtLocation
		(
			GetWorld(), 
			LightParticle,
			WeaponOwner->GetActorLocation() + WeaponOwner->GetActorForwardVector() * 600.f, 
			FRotator::ZeroRotator, 
			false
		);
	}

	GetWorldTimerManager().SetTimer(ExplosionTimer, this, &AShinbiSword::PlayExplosionEffect, 1.f, true);

	GetWorldTimerManager().SetTimer(UltimateWolfAttackTimer, this, &AShinbiSword::SpawnWolf, UltimateWolfAttackTime, 0.f);
}

void AShinbiSword::PlayExplosionEffect()
{
	const FVector RandLoc =
		WeaponOwner->GetActorLocation() +
		WeaponOwner->GetActorForwardVector() * 600.f +
		WeaponOwner->GetActorForwardVector() * FMath::RandRange(-100.f, 100.f) +
		WeaponOwner->GetActorRightVector() * FMath::RandRange(-100.f, 100.f);

	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, RandLoc, FRotator::ZeroRotator);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, RandLoc);
	}
}

void AShinbiSword::SpawnWolf()
{
	if (WolfProjClass == nullptr)
		return;

	if (CurrentTargetIdx == Enemies.Num())
	{
		CurrentTargetIdx = 0;
		CurrentAttackCycle++;
		if (CurrentAttackCycle == MaxAttackCycle)
		{
			CurrentAttackCycle = 0;
			LightParticleComp->Deactivate();

			if (LightEndParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation
				(
					GetWorld(),
					LightEndParticle,
					LightParticleComp->GetComponentLocation(),
					LightParticleComp->GetComponentRotation()
				);
			}

			GetWorldTimerManager().ClearTimer(ExplosionTimer);
			WeaponOwner->GetCombatComp()->UltimateWolfRushEnd();
			return;
		}
	}

	AEnemyCharacter* EC = Cast<AEnemyCharacter>(Enemies[CurrentTargetIdx]);
	if (EC->GetIsDead())
	{
		Enemies.RemoveAt(CurrentTargetIdx);
		SpawnWolf();
		return;
	}

	FVector RandLocation;
	FRotator RandRotation;

	GetRandomSpawnPosition(RandLocation, RandRotation);

	RandLocation += Enemies[CurrentTargetIdx]->GetActorLocation();

	if (PortalParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			GetWorld(), 
			PortalParticle, 
			RandLocation + FVector(50.f, 27.5f, -150.f),
			RandRotation
		);
	}
	if (PortalSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PortalSound, RandLocation + FVector(50.f, 0.f, -150.f));
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = WeaponOwner;
	SpawnParams.Instigator = WeaponOwner;

	AProjectile* SpawnedProj = GetWorld()->SpawnActor<AProjectile>
		(
			WolfProjClass, 
			RandLocation, 
			RandRotation, 
			SpawnParams
		);
	AProjectileShinbiWolf* WolfProjs = Cast<AProjectileShinbiWolf>(SpawnedProj);
	WolfProjs->UltimateWolfRushMode();

	CurrentTargetIdx++;
	GetWorldTimerManager().SetTimer(UltimateWolfAttackTimer, this, &AShinbiSword::SpawnWolf, UltimateWolfAttackTime);
}

void AShinbiSword::GetRandomSpawnPosition(FVector& SpawnLoc, FRotator& SpawnRot)
{
	int8 RandZPoint = FMath::RandRange(0, 8);
	int8 RandYPoint = FMath::RandRange(0, 1) ? FMath::RandRange(0, 2) : FMath::RandRange(7, 9);

	SpawnLoc = FVector(600.f, 0.f, 0.f).RotateAngleAxis(-20 * RandYPoint, FVector(0, 1, 0)).RotateAngleAxis(20 * RandZPoint, FVector(0, 0, 1));
	SpawnRot = FRotator(0.f, 20.f * RandZPoint, 0.f);
	
	if (RandYPoint == 7 || RandYPoint == 8)
	{
		SpawnRot.Roll += 180.f;
	}
	if (RandYPoint < 9)
	{
		SpawnRot.Pitch -= 20.f * RandYPoint;
		SpawnRot.Yaw += 180.f;
	}
}

