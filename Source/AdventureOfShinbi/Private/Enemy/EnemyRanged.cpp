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
#include "Sound/SoundCue.h"

AEnemyRanged::AEnemyRanged()
{
}

void AEnemyRanged::RangedAttack()
{
	CurrentFireCount++;
	PlayFireMontage();
	ProjectileFire();
}

void AEnemyRanged::BeginPlay()
{
	Super::BeginPlay();

	//EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyRanged::OnFireMontageEnded);
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
	
	// �������� �ʾ��� ��� Ÿ�� ������ TraceEnd �� ����
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

	bIsAttacking = true;

	int8 RandSectionNum = UKismetMathLibrary::RandomInteger(FireMontageSectionNameArr.Num());

	EnemyAnim->Montage_Play(FireMontage);

	EnemyAnim->Montage_JumpToSection(FireMontageSectionNameArr[RandSectionNum]);
}

void AEnemyRanged::OnFireMontageEnded()
{
	if (AIController)
	{
		bool bCheckCondition =
			GetCharacterMovement()->IsFalling() ||
			AIController->GetBlackBoard()->GetValueAsBool(FName("IsPlayerDead")) ||
			AIController->GetBlackBoard()->GetValueAsBool(FName("TargetIsVisible")) ? false : true;

		if (CurrentFireCount < FireCount)
		{
			RangedAttack();
		}
		else if(CurrentFireCount >= FireCount || bCheckCondition)
		{
			FinishFire();
		}
	}
}

void AEnemyRanged::FinishFire()
{
	if (AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("IsAttacking"), false);
	}
	CurrentFireCount = 0;
	bIsAttacking = false;
	OnAttackEnd.Broadcast();
}

AEnemyAIController* AEnemyRanged::GetEnemyController() const
{
	return AIController;
}
