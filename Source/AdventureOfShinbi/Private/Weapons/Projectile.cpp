

#include "Weapons/Projectile.h"
#include "Weapons/RangedWeapon.h"
#include "Enemy/EnemyCharacter.h"
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

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	ARangedWeapon* RW = Cast<ARangedWeapon>(GetOwner());
	if (RW)
	{
		Damage = RW->GetWeaponDamage();
		HeadShotDamage = RW->GetHeadShotDamage();
	}
	else
	{
		AEnemyCharacter* EC = Cast<AEnemyCharacter>(GetOwner());
		if (EC)
		{
			Damage = EC->GetEnemyDamage();
			HeadShotDamage = EC->GetEnemyDamage();
		}
	}
	
	BoxCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if (bIsPlayersProjectile)
	{
		BoxCollision->SetCollisionObjectType(ECC_PlayerProjectile);
		BoxCollision->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Block);
	}
	else
	{
		BoxCollision->SetCollisionObjectType(ECC_EnemyProjectile);
		BoxCollision->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Block);
	}

	IgnoreActors.Add(GetOwner());
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsExplosive)
	{
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
		float Dmg = Hit.BoneName == FName("head") ? HeadShotDamage : Damage;
		UGameplayStatics::ApplyPointDamage
		(
			OtherActor, 
			Dmg, 
			GetActorLocation(), 
			Hit,
			GetOwner()->GetInstigatorController(), 
			GetOwner(), 
			UDamageType::StaticClass()
		);
	}
}


