

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

	Damage = 200.f;
}

float AShinbiSword::GetSkillMana(EShinbiSkill ShinbiSkill)
{
	if (ShinbiSkill == EShinbiSkill::ESS_WolfAttack)
	{
		return WolfAttackMana;
	}
	else if (ShinbiSkill == EShinbiSkill::ESS_CirclingWolves)
	{
		return CirclingWolvesMana;
	}
	else if (ShinbiSkill == EShinbiSkill::ESS_UltimateWolfAttack)
	{
		return UltimateWolfAttackMana;
	}
	else
	{
		return 0.f;
	}
}

void AShinbiSword::BeginPlay()
{
	Super::BeginPlay();

	for (int8 i = 0; i < 5; i++)
	{
		ProjAngleOffset.Add(-72.f * i);
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

	// Wolf 투사체 Location, Rotation 업데이트
	if (CirclingWolvesArr.Num() > 0)
	{
		const FVector Loc = WeaponOwner->GetActorLocation();
		for (int8 i = 0; i < 5; i++)
		{
			const FVector RotatedLocValue = 
				FVector(300.f, 0.f, 0.f).RotateAngleAxis(ProjAngle + ProjAngleOffset[i], FVector(0,0,1));

			FRotator RotatedRotValue = CirclingWolvesArr[i]->GetActorRotation();
			RotatedRotValue += FRotator(0.f, RotRotateSpeed, 0.f) * DeltaTime;

			CirclingWolvesArr[i]->SetActorLocation(Loc + RotatedLocValue);
			CirclingWolvesArr[i]->SetActorRotation(RotatedRotValue);
		}

		ProjAngle += DeltaTime * LocRotateSpeed;

		if (ProjAngle >= 360.0f)
		{
			ProjAngle = 0;
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

	// 캐릭터 위치에서 투사체 스폰
	const FVector SpawnLocation = WeaponOwner->GetActorLocation() + (WeaponOwner->GetActorForwardVector() * 100.f) - FVector(0.f,0.f,40.f);
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

	PlayCirclingWolvesEffect();

	ProjAngle = 0.f;
	FRotator FirstRot(0.f, -18.f, 0.f);

	// Wolf 투사체 스폰 위치 계산 후 스폰
	for (int8 i = 0; i < 5; i++)
	{
		const FVector RotateValue = FVector(300.f, 0.f, 0.f).RotateAngleAxis(-72.f * i, FVector(0,0,1));

		FirstRot += FRotator(0.f, -72.f, 0.f);

		AProjectile* SpawnedProj = GetWorld()->SpawnActor<AProjectile>
			(WolfProjClass, WeaponOwner->GetActorLocation() + RotateValue, FirstRot, SpawnParams);
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
	if (WeaponOwner == nullptr)
	{
		WeaponOwner = Cast<AAOSCharacter>(GetOwner());
	}

	// 모든 적 액터 가져오기
	Enemies.Empty();
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyCharacter::StaticClass(), Enemies);

	// 조건을 만족하지 않는 적 액터 삭제
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

	// 공격 사이클 완료 후
	if (CurrentTargetIdx == Enemies.Num())
	{
		CurrentTargetIdx = 0;
		CurrentAttackCycle++;

		// 마지막 공격 사이클인 경우 
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

	if (
		IsValid(Enemies[CurrentTargetIdx]) == false || 
		Cast<AEnemyCharacter>(Enemies[CurrentTargetIdx])->GetIsDead()
		)
	{
		Enemies.RemoveAt(CurrentTargetIdx);
		SpawnWolf();
		return;
	}

	FVector RandLocation;
	FRotator RandRotation;

	// 투사체가 소환될 랜덤 Location, Rotation 계산
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

