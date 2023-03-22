

#include "System/ObjectPool.h"
#include "Weapons/Projectile.h"

UObjectPool::UObjectPool()
{
	PrimaryComponentTick.bCanEverTick = false;

}

//AProjectile* UObjectPool::GetPooledActor()
//{
//	for (AProjectile* PooledActor : Pool)
//	{
//		if (!PooledActor->GetIsActive())
//			return PooledActor;
//	}
//	return nullptr;
//}

AProjectile* UObjectPool::GetPooledActor()
{
	if (DeactivatedNum == 0) return nullptr;
	if (ActivatedNum == 0) ActivatedNum = PoolSize;
	
	DeactivatedNum--;
	return Pool[--ActivatedNum];
}

void UObjectPool::StartPooling()
{
	if (ThisOwner == nullptr) return;

	APawn* InstigatorPawn = Cast<APawn>(ThisOwner);
	if (InstigatorPawn == nullptr) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = ThisOwner;
	SpawnParams.Instigator = InstigatorPawn;

	if (ProjectileSubclass)
	{
		UWorld* World = GetWorld();
		if (World == nullptr) return;
		for (int i = 0; i < PoolSize; i++)
		{
			AProjectile* PooledActor = GetWorld()->SpawnActor<AProjectile>(ProjectileSubclass, FVector().ZeroVector, FRotator().ZeroRotator, SpawnParams);
			PooledActor->DDeactivatePooledObject.BindUObject(this, &UObjectPool::AddDeactivatedNum);
			Pool.Add(PooledActor);
		}
	}
}

void UObjectPool::DestroyPool()
{
	for (AProjectile* PooledActor : Pool)
	{
		PooledActor->Destroy();
	}
	Pool.Empty();
}

void UObjectPool::SetProjectileDmg(float Dmg)
{
	for (AProjectile* PooledActor : Pool)
	{
		PooledActor->SetDamage(Dmg);
	}
}

void UObjectPool::BeginPlay()
{
	Super::BeginPlay();

	ActivatedNum = DeactivatedNum = PoolSize;
}

void UObjectPool::AddDeactivatedNum()
{
	DeactivatedNum++;
}
