

#include "Weapons/Projectile.h"
#include "Weapons/RangedWeapon.h"
#include "Player/AOSCharacter.h"
#include "Enemy/EnemyCharacter.h"
#include "Components/CombatComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "../AdventureOfShinbi.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetGenerateOverlapEvents(true);
	BoxCollision->SetNotifyRigidBodyCollision(true);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	BoxCollision->SetEnableGravity(false);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 10000.f;
	ProjectileMovementComponent->MaxSpeed = 10000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->SetupAttachment(BoxCollision);
	RadialForce->Radius = 500.f;
	RadialForce->bImpulseVelChange = true;
	RadialForce->bIgnoreOwningActor = true;
	RadialForce->bAutoActivate = true;
}

void AProjectile::SetDamage(float Value)
{
	Damage = Value;
	HeadShotDamage = Value * 1.5f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	AEnemyCharacter* EC = Cast<AEnemyCharacter>(GetInstigator());
	if (EC)
	{
		Damage = EC->GetEnemyDamage();
		HeadShotDamage = EC->GetEnemyDamage();
	}
	
	BoxCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if (bIsPlayersProjectile)
	{
		BoxCollision->SetCollisionObjectType(ECC_PlayerProjectile);
		BoxCollision->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Ignore);
		AAOSCharacter* AC = Cast<AAOSCharacter>(GetInstigator());
		if (AC)
		{
			Damage = AC->GetCombatComp()->GetDmgDebuffActivated() ? Damage * 0.7 : Damage;
		}
	}
	else
	{
		BoxCollision->SetCollisionObjectType(ECC_EnemyProjectile);
		BoxCollision->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Ignore);
	}

	IgnoreActors.Add(GetOwner());
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (bIsExplosive)
	{
		if (CameraShake)
		{
			UGameplayStatics::PlayWorldCameraShake(this, CameraShake, GetActorLocation(), 200.f, CameraShakeRadius);
		}

		UGameplayStatics::ApplyRadialDamage
		(
			this, 
			Damage, 
			GetActorLocation(), 
			ExplosionRadius, 
			UDamageType::StaticClass(), 
			IgnoreActors, 
			GetOwner(),
			GetOwner()->GetInstigatorController()
		);
		RadialForce->FireImpulse();
	}
	else
	{
		if (CameraShake)
		{
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShake);
		}

		const float Dmg = Hit.BoneName == FName("head") ? Damage * 1.5f : Damage;
		AActor* DmgCauser = bIsPlayersProjectile ? GetOwner() : this;
		UGameplayStatics::ApplyPointDamage
		(
			OtherActor, 
			Dmg, 
			GetActorLocation(), 
			Hit,
			GetOwner()->GetInstigatorController(), 
			DmgCauser,
			UDamageType::StaticClass()
		);
	}
}

