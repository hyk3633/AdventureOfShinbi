

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
#include "DrawDebugHelpers.h"
AEnemyRanged::AEnemyRanged()
{
	WeaponTraceStartSocketName = FName("RangedTraceStart");
	WeaponTraceEndSocketName = FName("RangedTraceEnd");

	Damage = 250.f;
	Health = 600.f;
	MaxHealth = 600.f;
	Defense = 20.f;
	DefaultValue = 10.f;
	RandRangeValue = 10;

	AssetName = TEXT("Ranged_Core");
}

void AEnemyRanged::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyRanged::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyRanged::AbortAttack()
{
	Super::AbortAttack();

	CurrentFireCount = 0;
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
	if (MuzzleSocket == nullptr) 
		return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetMesh());

	FVector ToTarget = HitPoint - SocketTransform.GetLocation();
	FRotator TargetRotation = ToTarget.Rotation();

	if (Projectile)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
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
	// AI의 시각 위치 값을 받아옴
	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);

	// 타겟 플레이어가 설정되어 있으면 타겟 방향으로 설정
	FVector ShotDirection;
	if (AiInfo.TargetPlayer)
	{
		ShotDirection = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AiInfo.TargetPlayer->GetActorLocation()).Vector();
	}
	else
	{
		ShotDirection = EyeRotation.Vector();
	}

	// 탄 퍼짐
	float HalfRad = FMath::DegreesToRadians(BulletSpread);
	ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

	FVector TraceEnd = EyeLocation + (ShotDirection * 10000.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult HitResult;

	GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);

	// 트레이스가 적중하지 않았을 경우 투사체가 향할 지점을 TraceEnd로 지정
	if (HitResult.bBlockingHit == false)
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
	if (EnemyAnim == nullptr || FireMontage == nullptr) 
		return;

	int8 RandSectionNum = UKismetMathLibrary::RandomInteger(FireMontageSectionNameArr.Num());

	EnemyAnim->Montage_Play(FireMontage);

	EnemyAnim->Montage_JumpToSection(FireMontageSectionNameArr[RandSectionNum]);
}

void AEnemyRanged::OnFireMontageEnded()
{
	CurrentFireCount++;

	bool bCheckCondition =
		GetCharacterMovement()->IsFalling() ||	// 공중에 떠 있거나
		GetAiInfo().bIsPlayerDead ||			// 플레이어가 사망했거나
		GetAiInfo().bTargetIsVisible == false;	// 타겟이 보이지 않으면 원거리 공격 중단

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
