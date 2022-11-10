
#include "Weapons/ProjectileShinbiWolf.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

AProjectileShinbiWolf::AProjectileShinbiWolf()
{
	PrimaryActorTick.bCanEverTick = true;

	WolfMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Wolf Mesh"));
	WolfMesh->SetupAttachment(RootComponent);
	WolfMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent->InitialSpeed = 1000.f;
	ProjectileMovementComponent->MaxSpeed = 1000.f;

	bIsPlayersProjectile = true;

	Damage = 50.f;
}

void AProjectileShinbiWolf::BeginPlay()
{
	Super::BeginPlay();

}

void AProjectileShinbiWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileShinbiWolf::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HittedEnemy == OtherActor)
		return;

	HittedEnemy = OtherActor;

	UGameplayStatics::ApplyPointDamage
	(
		OtherActor,
		Damage,
		GetActorLocation(),
		SweepResult,
		GetOwner()->GetInstigatorController(),
		GetOwner(),
		UDamageType::StaticClass()
	);

	if (SmallImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SmallImpactSound, GetActorLocation());
	}
	if (CirclingWolvesImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CirclingWolvesImpactParticle, GetActorLocation(), GetActorLocation().Rotation(), true);
	}

	GetWorldTimerManager().SetTimer(InitHittedEnemyTimer, this, &AProjectileShinbiWolf::InitHittedEnemy, 0.25f);
}

void AProjectileShinbiWolf::InitHittedEnemy()
{
	HittedEnemy = nullptr;
}

void AProjectileShinbiWolf::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	GetWorldTimerManager().SetTimer(DestroyDelayTimer, this, &AProjectileShinbiWolf::DestoryProj, DestroyDelayTime);

	WolfMesh->SetVisibility(false);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent->StopMovementImmediately();

	if (NormalImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, NormalImpactSound, Hit.ImpactPoint);
	}
	if (WolfAttackImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WolfAttackImpactParticle, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), true);
	}
}

void AProjectileShinbiWolf::WolfAttackMode()
{
	GetWorldTimerManager().SetTimer(LifeTimer, this, &AProjectileShinbiWolf::LifeOver, LifeTime);

	WolfMode = EShinbiWolfMode::ESW_WolfAttack;

	if (WolfAttackTrail)
	{
		WolfAttackTrailComp = UGameplayStatics::SpawnEmitterAttached
		(
			WolfAttackTrail,
			WolfMesh,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset
		);
	}

	if (WolfAttackMontage)
	{
		WolfMesh->GetAnimInstance()->Montage_Play(WolfAttackMontage);
	}
}

void AProjectileShinbiWolf::CirclingWolvesMode()
{
	ProjectileMovementComponent->bRotationFollowsVelocity = false;

	WolfMode = EShinbiWolfMode::ESW_CirclingWolves;

	BoxCollision->SetNotifyRigidBodyCollision(false);
	BoxCollision->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Overlap);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectileShinbiWolf::OnOverlap);

	if (CirclingWolvesAnim)
	{
		WolfMesh->PlayAnimation(CirclingWolvesAnim, true);
	}

	if (CirclingWolvesTrail)
	{
		const USkeletalMeshSocket* MuzzleSocket = WolfMesh->GetSocketByName(FName("TailSocket"));
		if (MuzzleSocket == nullptr) return;
		const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(WolfMesh);

		CirclingWolvesTrailComp = UGameplayStatics::SpawnEmitterAttached
		(
			CirclingWolvesTrail,
			WolfMesh,
			NAME_None,
			SocketTransform.GetLocation(),
			SocketTransform.GetRotation().Rotator(),
			EAttachLocation::KeepWorldPosition
		);
	}
}

void AProjectileShinbiWolf::CirclingWolvesEnd()
{
	LifeOver();
}

void AProjectileShinbiWolf::UltimateWolfRushMode()
{
	GetWorldTimerManager().SetTimer(LifeTimer, this, &AProjectileShinbiWolf::LifeOver, LifeTime);

	WolfMode = EShinbiWolfMode::ESW_UltimateWolf;

	if (WolfAttackTrail)
	{
		WolfAttackTrailComp = UGameplayStatics::SpawnEmitterAttached
		(
			WolfAttackTrail,
			WolfMesh,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset
		);
	}

	if (UltimateWolfRushAnim)
	{
		WolfMesh->PlayAnimation(UltimateWolfRushAnim, false);
	}
}

void AProjectileShinbiWolf::WolfJump()
{
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->AddForce(BoxCollision->GetUpVector() * 30000.f * BoxCollision->GetMass());
}

void AProjectileShinbiWolf::LifeOver()
{
	WolfMesh->SetVisibility(false);
	ProjectileMovementComponent->StopMovementImmediately();

	if (WolfMode == EShinbiWolfMode::ESW_WolfAttack)
	{
		BoxCollision->SetSimulatePhysics(false);
		WolfAttackTrailComp->Deactivate();
		if (WolfAttackNoHitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WolfAttackNoHitParticle, GetActorLocation(), GetActorRotation());
		}
	}
	else if (WolfMode == EShinbiWolfMode::ESW_CirclingWolves)
	{
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WolfMesh->Stop();
		CirclingWolvesTrailComp->Deactivate();
		if (WolfRemovalParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WolfRemovalParticle, GetActorLocation(), GetActorRotation());
		}
	}
	else if (WolfMode == EShinbiWolfMode::ESW_UltimateWolf)
	{
		WolfAttackTrailComp->Deactivate();
		if (UltimateWolfEndParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UltimateWolfEndParticle, GetActorLocation(), GetActorRotation());
		}
	}

	GetWorldTimerManager().SetTimer(DestroyDelayTimer, this, &AProjectileShinbiWolf::DestoryProj, DestroyDelayTime);
}

void AProjectileShinbiWolf::DestoryProj()
{
	Destroy();
}