

#include "Weapons/RangedProjectileWeapon.h"
#include "Weapons/Projectile.h"
#include "Weapons/ProjectileBullet.h"
#include "Engine/SkeletalMeshSocket.h"
#include "System/ObjectPool.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

ARangedProjectileWeapon::ARangedProjectileWeapon()
{
	ObjectPooler = CreateDefaultSubobject<UObjectPool>(TEXT("Bullet Pooler"));
	LoadedAmmo = 100;
}

void ARangedProjectileWeapon::SetWeaponState(const EWeaponState State)
{
	Super::SetWeaponState(State);

	if (State == EWeaponState::EWS_Equipped)
	{
		ObjectPooler->SetOwner(GetOwner());
		ObjectPooler->StartPooling();
	}
	else if (State == EWeaponState::EWS_Field)
	{
		ObjectPooler->DestroyPool();
	}
}

void ARangedProjectileWeapon::Firing()
{
	ARangedWeapon::Firing();
	PlayFireEffect(MuzzleFlashParticle, FireSound);

	if (bScatterGun)
	{
		ScatterFiring(ProjectileClass);
	}
	else
	{
		//SingleFiring(ProjectileClass);
		SingleFiring();
	}

	ConsumeAmmo();
}

void ARangedProjectileWeapon::ScatterFiring(TSubclassOf<AProjectile> Projectile)
{
	if (Projectile == nullptr)
		return;

	FVector LocationToSpawn;
	GetSpawnLocation(LocationToSpawn);

	FVector HitPoint;
	CrosshairLineTrace(HitPoint);

	// 랜덤한 총알 발사 벡터를 탄알 수 만큼 저장
	TArray<FRotator> ShotRotator;
	for (int8 i = 0; i < NumberOfShots; i++)
	{
		const FVector ToTarget = HitPoint - LocationToSpawn;
		const FVector RandomUnitVector = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ToTarget, ScatterRange);
		ShotRotator.Add(RandomUnitVector.Rotation());
	}

	for (int8 i = 0; i < NumberOfShots; i++)
	{
		SpawnProjectile(Projectile, LocationToSpawn, ShotRotator[i]);
	}
}

void ARangedProjectileWeapon::SingleFiring(TSubclassOf<AProjectile> Projectile)
{
	if (Projectile == nullptr)
		return;

	FVector LocationToSpawn;
	GetSpawnLocation(LocationToSpawn);

	FRotator RotationToSpawn;
	GetSpawnRotation(LocationToSpawn, RotationToSpawn);

	SpawnProjectile(Projectile, LocationToSpawn, RotationToSpawn);
}

void ARangedProjectileWeapon::SingleFiring()
{
	AProjectile* PooledActor = ObjectPooler->GetPooledActor();
	if (PooledActor == nullptr) return;

	FVector LocationToSpawn;
	GetSpawnLocation(LocationToSpawn);

	FRotator RotationToSpawn;
	GetSpawnRotation(LocationToSpawn, RotationToSpawn);

	PooledActor->SetActorLocation(LocationToSpawn);
	PooledActor->SetActorRotation(RotationToSpawn);
	PooledActor->SetLifeSpan(LifeSpan);
	PooledActor->Activate();
}

void ARangedProjectileWeapon::SpawnProjectile(TSubclassOf<AProjectile> Projectile, const FVector& LocToSpawn, const FRotator& RotToSpawn)
{
	if (GetOwner() == nullptr)
		return;

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
			World->SpawnActor<AProjectile>(Projectile, LocToSpawn, RotToSpawn, SpawnParams);
		}
	}
}

void ARangedProjectileWeapon::GetSpawnLocation(FVector& ProjLoc)
{
	// 총구 소켓 위치 반환
	const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetItemMesh());

	ProjLoc = SocketTransform.GetLocation();
}

void ARangedProjectileWeapon::GetSpawnRotation(const FVector& ProjLoc, FRotator& ProjRot)
{
	// 총구 소켓 위치에서 크로스헤어 위치에서 수행한 트레이스의 적중 위치로 향하는 Rotation 반환 
	FVector HitPoint;
	CrosshairLineTrace(HitPoint);

	FVector ToTarget = HitPoint - ProjLoc;
	ProjRot = ToTarget.Rotation();
}
