// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyRanged.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyAnimInstance.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Player/AOSCharacter.h"
#include "Weapons/Projectile.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"

AEnemyRanged::AEnemyRanged()
{
	WeaponTraceStartSocketName = FName("RangedTraceStart");
	WeaponTraceEndSocketName = FName("RangedTraceEnd");
}

void AEnemyRanged::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyRanged::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyRanged::RangedAttack()
{
	bIsAttacking = true;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	GetWorldTimerManager().SetTimer(FireDelayTimer, this, &AEnemyRanged::AfterFireDelay, FireDelayTime, false);
}

void AEnemyRanged::ProjectileFire(TSubclassOf<AProjectile> Projectile)
{
	FVector HitPoint;
	CrosshairLineTrace(HitPoint);

	const USkeletalMeshSocket* MuzzleSocket = GetMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetMesh());

	FVector ToTarget = HitPoint - SocketTransform.GetLocation();
	FRotator TargetRotation = ToTarget.Rotation();

	if (Projectile)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = this;

		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(Projectile, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
		}
	}
}

void AEnemyRanged::CrosshairLineTrace(FVector& OutHitPoint)
{
	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();

	// Bullet Spread
	float HalfRad = FMath::DegreesToRadians(BulletSpread);
	ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

	FVector TraceEnd = EyeLocation + (ShotDirection * 10000.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult HitResult;

	GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);
	
	// 적중하지 않았을 경우 타격 지점을 TraceEnd 로 지정
	if (!HitResult.bBlockingHit)
	{
		OutHitPoint = TraceEnd;
	}
	else
	{
		OutHitPoint = HitResult.ImpactPoint;
	}
}

void AEnemyRanged::PlayFireMontage()
{
	if (EnemyAnim == nullptr || FireMontage == nullptr) return;

	int8 RandSectionNum = UKismetMathLibrary::RandomInteger(FireMontageSectionNameArr.Num());

	EnemyAnim->Montage_Play(FireMontage);

	EnemyAnim->Montage_JumpToSection(FireMontageSectionNameArr[RandSectionNum]);
}

void AEnemyRanged::OnFireMontageEnded()
{
	CurrentFireCount++;

	bool bCheckCondition =
		GetCharacterMovement()->IsFalling() ||
		GetAiInfo().bIsPlayerDead ||
		GetAiInfo().bTargetIsVisible == false;

	if (CurrentFireCount == FireCount || bCheckCondition)
	{
		FinishFire();
	}
	else
	{
		RangedAttack();
	}
}

void AEnemyRanged::FinishFire()
{
	CurrentFireCount = 0;
	bIsAttacking = false;
	if (GetAttackRange()->IsOverlappingActor(AiInfo.TargetPlayer) == false)
		AiInfo.bTargetInAttackRange = false;
	else
		AiInfo.bTargetInAttackRange = true;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	OnAttackEnd.Broadcast();
}

void AEnemyRanged::AfterFireDelay()
{
	PlayFireMontage();
	ProjectileFire(ProjectileClass);
}

AEnemyAIController* AEnemyRanged::GetEnemyController() const
{
	return AIController;
}
