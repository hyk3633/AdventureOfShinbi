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

AEnemyRanged::AEnemyRanged()
{

}

void AEnemyRanged::Attack()
{
	if (AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("IsAttacking"), true);
	}
	IsAttacking = true;
	OnAttackEnd.Broadcast();

	if (bTargetIsInAttackRange)
	{
		PlayAttackMontage();
	}
	else if (bTargetIsInChaseRange)
	{
		RangedAttack();
	}
}

void AEnemyRanged::RangedAttack()
{
	if (GetCharacterMovement()->IsFalling())
	{
		CurrentFireCount = 0;
		return;
	}
	if (AIController)
	{
		if (AIController->GetBlackBoard()->GetValueAsBool(FName("IsPlayerDead")))
		{
			return;
		}
	}
	CurrentFireCount++;
	PlayFireMontage();
	ProjectileFire();
}

void AEnemyRanged::BeginPlay()
{
	Super::BeginPlay();

	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyRanged::OnFireMontageEnded);
}

void AEnemyRanged::OnChaseRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnChaseRangeOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	bTargetIsInChaseRange = true;
}

void AEnemyRanged::OnChaseRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnChaseRangeEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	bTargetIsInChaseRange = false;
}

void AEnemyRanged::OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnChaseRangeOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	bTargetIsInAttackRange = true;
}

void AEnemyRanged::OnAttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnChaseRangeEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	bTargetIsInAttackRange = false;
}

void AEnemyRanged::ProjectileFire()
{
	FVector HitPoint;
	CrosshairLineTrace(HitPoint);

	const USkeletalMeshSocket* MuzzleSocket = GetMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetMesh());

	FVector ToTarget = HitPoint - SocketTransform.GetLocation();
	FRotator TargetRotation = ToTarget.Rotation();

	if (ProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = this;

		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
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

		if (AIController)
		{
			AIController->GetBlackBoard()->SetValueAsObject(FName("AimedTarget"), nullptr);
		}
	}
	else
	{
		OutHitPoint = HitResult.ImpactPoint;

		if (AIController)
		{
			AAOSCharacter* Cha = Cast<AAOSCharacter>(HitResult.GetActor());
			if (Cha)
			{
				AIController->GetBlackBoard()->SetValueAsObject(FName("AimedTarget"), Cha);
			}
			else
			{
				AIController->GetBlackBoard()->SetValueAsObject(FName("AimedTarget"), nullptr);
			}
		}
	}
	
}

void AEnemyRanged::PlayFireMontage()
{
	if (EnemyAnim == nullptr || FireMontage == nullptr) return;

	int8 RandSectionNum = UKismetMathLibrary::RandomInteger(FireMontageSectionNameArr.Num());

	EnemyAnim->Montage_Play(FireMontage);

	EnemyAnim->Montage_JumpToSection(FireMontageSectionNameArr[RandSectionNum]);
}

void AEnemyRanged::OnFireMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == FireMontage && AIController)
	{
		if (CurrentFireCount < FireCount)
		{
			RangedAttack();
		}
		else
		{
			AIController->GetBlackBoard()->SetValueAsBool(FName("IsAttacking"), false);
			CurrentFireCount = 0;
			IsAttacking = false;
			OnAttackEnd.Broadcast();
		}
	}
}
