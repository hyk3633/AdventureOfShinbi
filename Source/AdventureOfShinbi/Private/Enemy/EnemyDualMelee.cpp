// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyDualMelee.h"
#include "Enemy/EnemyAIController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

AEnemyDualMelee::AEnemyDualMelee()
{

}

void AEnemyDualMelee::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyDualMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Weapon2LineTrace();
}

void AEnemyDualMelee::Weapon2LineTrace()
{
	if (bActivateWeaponTrace2)
	{
		const USkeletalMeshSocket* WeaponTraceStart = GetMesh()->GetSocketByName("Weapon2TraceStart");
		if (WeaponTraceStart == nullptr) return;
		const FTransform SocketTransformStart = WeaponTraceStart->GetSocketTransform(GetMesh());

		const USkeletalMeshSocket* WeaponTraceEnd = GetMesh()->GetSocketByName("Weapon2TraceEnd");
		if (WeaponTraceEnd == nullptr) return;
		const FTransform SocketTransformEnd = WeaponTraceEnd->GetSocketTransform(GetMesh());

		FHitResult WeaponHitResult;
		FVector TraceStart = SocketTransformStart.GetLocation();
		FVector TraceEnd = SocketTransformEnd.GetLocation();

		GetWorld()->LineTraceSingleByChannel(WeaponHitResult, TraceStart, TraceEnd, ECC_EnemyWeaponTrace);

		if (WeaponHitResult.bBlockingHit)
		{
			PlayMeleeAttackEffect(WeaponHitResult.ImpactPoint, WeaponHitResult.ImpactNormal.Rotation());

			if (bIsAttacking && AIController)
			{
				UGameplayStatics::ApplyPointDamage(WeaponHitResult.GetActor(), Damage, WeaponHitResult.ImpactPoint, WeaponHitResult, AIController, this, UDamageType::StaticClass());
			}

			bActivateWeaponTrace2 = false;
		}
	}
}

void AEnemyDualMelee::ActivateWeaponTrace2()
{
	bActivateWeaponTrace2 = true;
}

