

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

	Weapon2BoxTrace();
}

void AEnemyDualMelee::Weapon2BoxTrace()
{
	if (bActivateWeaponTrace2)
	{
		FHitResult WeaponHitResult;
		GetBoxTraceHitResult(WeaponHitResult, FName("Weapon2TraceStart"), FName("Weapon2TraceEnd"));

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

void AEnemyDualMelee::DeactivateWeaponTrace2()
{
	bActivateWeaponTrace2 = false;
}

