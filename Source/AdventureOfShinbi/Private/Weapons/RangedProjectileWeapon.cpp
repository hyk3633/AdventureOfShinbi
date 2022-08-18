

#include "Weapons/RangedProjectileWeapon.h"
#include "Weapons/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

void ARangedProjectileWeapon::Firing()
{
	PlayFireEffect();

	FVector HitPoint;
	CrosshairLineTrace(HitPoint);

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());

	FVector ToTarget = HitPoint - SocketTransform.GetLocation();
	FRotator TargetRotation = ToTarget.Rotation();

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	if (ProjectileClass && InstigatorPawn)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
		}
	}
}

void ARangedProjectileWeapon::ScatterFiring()
{
	PlayFireEffect();

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());

	FVector HitPoint;
	CrosshairLineTrace(HitPoint);

	for (int8 i = 0; i < NumberOfShots; i++)
	{
		FVector ToTarget = HitPoint - SocketTransform.GetLocation();
		FVector RandomUnitVector = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ToTarget, ScatterRange);
		ShotRotator.Add(RandomUnitVector.Rotation());
	}

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	if (ProjectileClass && InstigatorPawn)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		UWorld* World = GetWorld();
		if (World)
		{
			for (int8 i = 0; i < NumberOfShots; i++)
			{
				World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), ShotRotator[i], SpawnParams);
			}
		}
	}

	ShotRotator.Empty();
}
