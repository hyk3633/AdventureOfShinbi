// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/MeleeWeapon.h"
#include "Components/BoxComponent.h"
#include "Player/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

AMeleeWeapon::AMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	WeaponCapsuleTrace();
}

bool AMeleeWeapon::WeaponCapsuleTrace()
{
	if (bActivateWeaponTrace == false)
		return false;

	TArray<FHitResult> WeaponHitResults;
	const bool bHitSomething = GetCapsuleTraceHitResult(WeaponHitResults);

	if (bHitSomething == false)
		return false;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	
	TArray<AActor*> HitEnemies;
	for (FHitResult Result : WeaponHitResults)
	{
		if (HitEnemies.Contains(Result.GetActor()) == false)
		{
			HitEnemies.Add(Result.GetActor());

			PlayAttackEffect(Result.ImpactPoint, Result.ImpactNormal.Rotation());

			UGameplayStatics::ApplyPointDamage
			(
				Result.GetActor(),
				Damage,
				Result.ImpactPoint,
				Result,
				OwnerPawn->GetController(),
				this,
				UDamageType::StaticClass()
			);
		}
	}

	DeactivateWeaponTrace();

	return true;
}

bool AMeleeWeapon::GetCapsuleTraceHitResult(TArray<FHitResult>& HitResults)
{
	const USkeletalMeshSocket* WeaponTraceStart = ItemMesh->GetSocketByName("WeaponTraceStart");
	if (WeaponTraceStart == nullptr) 
		return false;

	const USkeletalMeshSocket* WeaponTraceEnd = ItemMesh->GetSocketByName("WeaponTraceEnd");
	if (WeaponTraceEnd == nullptr) 
		return false;

	UCollisionProfile* Profile = UCollisionProfile::Get();
	ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECC_PlayerWeaponTrace);

	const bool bHitSomething = UKismetSystemLibrary::CapsuleTraceMulti
	(
		this,
		WeaponTraceStart->GetSocketTransform(ItemMesh).GetLocation(),
		WeaponTraceEnd->GetSocketTransform(ItemMesh).GetLocation(),
		CapsuleRadius,
		CapsuleHalfHeight,
		TraceType,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResults,
		true
	);

	return bHitSomething;
}

void AMeleeWeapon::PlayAttackEffect(const FVector& Location, const FRotator& Rotation)
{
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location, Rotation);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location);
	}
}

void AMeleeWeapon::ActivateWeaponTrace()
{
	bActivateWeaponTrace = true;
}

void AMeleeWeapon::DeactivateWeaponTrace()
{
	bActivateWeaponTrace = false;
}