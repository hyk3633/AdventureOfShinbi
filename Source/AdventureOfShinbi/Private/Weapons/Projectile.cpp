

#include "Weapons/Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AOSCharacter.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	BoxCollision->SetEnableGravity(false);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	ProjectileMovementComponent->InitialSpeed = 10000.f;
	ProjectileMovementComponent->MaxSpeed = 10000.f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	IgnoreActors.Add(GetOwner());
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AAOSCharacter* Causer = Cast<AAOSCharacter>(GetOwner());
	if (Causer)
	{
		AController* CauserController = Cast<AController>(Causer->Controller);
		if (CauserController)
		{
			if (bIsExplosive)
			{
				UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation(), ExplosionRadius, UDamageType::StaticClass(), IgnoreActors, Causer, CauserController);
			}
			else
			{
				float Dmg = Hit.BoneName == FName("head") ? HeadShotDamage : Damage;
				UGameplayStatics::ApplyDamage(OtherActor, Dmg, CauserController, Causer, UDamageType::StaticClass());
			}
		}
	}
}


