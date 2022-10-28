

#include "Weapons/RangedProjectileWeapon.h"
#include "Weapons/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

void ARangedProjectileWeapon::Firing()
{
	PlayFireEffect(MuzzleFlashParticle, FireSound);

	SpawnProjectile(ProjectileClass);
}

void ARangedProjectileWeapon::ScatterFiring()
{
	PlayFireEffect(MuzzleFlashParticle, FireSound);

	const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetItemMesh());

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

void ARangedProjectileWeapon::SpawnProjectile(TSubclassOf<AProjectile> Projectile)
{
	FVector HitPoint;
	CrosshairLineTrace(HitPoint);

	const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetItemMesh());

	FVector ToTarget = HitPoint - SocketTransform.GetLocation();
	FRotator TargetRotation = ToTarget.Rotation();

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (InstigatorPawn == nullptr)
		return;

	if (Projectile)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(Projectile, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
		}
	}
}

bool ARangedProjectileWeapon::GetScatterGun() const
{
	return bScatterGun;
}
