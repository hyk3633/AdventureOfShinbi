

#include "Weapons/RangedWeapon.h"
#include "EnemyBoss/EnemyBoss.h"
#include "Player/AOSCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/ItemAmmo.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"

ARangedWeapon::ARangedWeapon()
{

}

void ARangedWeapon::Firing()
{
	if (CameraShakeGunFire)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShakeGunFire);
	}
}

void ARangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
	}

	if (AmmoClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			AmmoItem = World->SpawnActor<AItem>(AmmoClass);
		}
	}

	if (AmmoItem)
	{
		AItemAmmo* IA = Cast<AItemAmmo>(AmmoItem);
		if (IA)
		{
			IA->GetStaticMesh()->SetVisibility(false);
		}
	}
}

void ARangedWeapon::CrosshairLineTrace(FVector& OutHitPoint)
{
	FVector2D CrosshairLocation(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FHitResult HitResult;
		FVector TraceStart = CrosshairWorldPosition;
		FVector TraceEnd = TraceStart + CrosshairWorldDirection * 10000.f;

		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

		// 적중하지 않았을 경우 타격 지점을 TraceEnd 로 지정
		if (!HitResult.bBlockingHit)
		{
			OutHitPoint = TraceEnd;
		}
		else
		{
			OutHitPoint = HitResult.ImpactPoint;

			/*AEnemyBoss* Boss = Cast<AEnemyBoss>(HitResult.GetActor());
			if (Boss)
			{
				AAOSCharacter* WeaponOwner = Cast<AAOSCharacter>(GetOwner());
				if (WeaponOwner)
				{
					WeaponOwner->DAttackButtonPressed.ExecuteIfBound();
					UE_LOG(LogTemp, Warning, TEXT("target hit"));
				}
			}*/
		}
	}
}

void ARangedWeapon::PlayFireEffect(UParticleSystem* Flash, USoundCue* Sound)
{
	if (Flash)
	{
		const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName("MuzzleSocket");
		if (MuzzleSocket)
		{
			const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetItemMesh());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Flash, SocketTransform);
		}
	}
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound);
	}
}

AItem* ARangedWeapon::GetAmmoItem() const
{
	return AmmoItem;
}

ERangedWeaponType ARangedWeapon::GetRangedWeaponType() const
{
	return RangedWeaponType;
}

float ARangedWeapon::GetHeadShotDamage()
{
	return GetWeaponDamage() * DamageMultiplier;
}

EAmmoType ARangedWeapon::GetAmmoType() const
{
	return AmmoType;
}

int32 ARangedWeapon::GetMagazine() const
{
	return Magazine;
}

int32 ARangedWeapon::GetLoadedAmmo() const
{
	return LoadedAmmo;
}

void ARangedWeapon::SetLoadedAmmo(const int32 Quantity)
{
	LoadedAmmo = Quantity;
}

float ARangedWeapon::GetGunRecoil() const
{
	return GunRecoil;
}

bool ARangedWeapon::GetAutomaticFire() const
{
	return bAutomaticFire;
}

float ARangedWeapon::GetFireRate() const
{
	return FireRate;
}

float ARangedWeapon::GetZoomScope() const
{
	return ZoomScope;
}

void ARangedWeapon::ConsumeAmmo()
{
	LoadedAmmo = FMath::Clamp(LoadedAmmo - AmmoConsumption, 0, Magazine);
}

void ARangedWeapon::PlayNoAmmoSound()
{
	if (NoAmmoSound)
	{
		UGameplayStatics::PlaySound2D(this, NoAmmoSound);
	}
}
