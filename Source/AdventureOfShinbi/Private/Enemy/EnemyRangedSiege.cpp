

#include "Enemy/EnemyRangedSiege.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/EnemyAIController.h"
#include "Player/AOSCharacter.h"
#include "Weapons/Projectile.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Engine/SkeletalMeshSocket.h"

AEnemyRangedSiege::AEnemyRangedSiege()
{
	BoxTraceSize = FVector(40.f, 30.f, 40.f);

	Damage = 400.f;
	Health = 1200.f;
	MaxHealth = 1200.f;
	Defense = 45.f;
	DefaultValue = 5.f;
	RandRangeValue = 10;
}

void AEnemyRangedSiege::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyRangedSiege::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyRangedSiege::HandleStiffAndStun(bool IsHeadShot)
{
	if (EnemyState == EEnemyState::EES_Siege)
		return;

	Super::HandleStiffAndStun(IsHeadShot);
}

bool AEnemyRangedSiege::CheckRotateToTargetCondition()
{
	return bDeath == false &&
		AiInfo.bStiffed == false &&
		AiInfo.bStunned == false &&
		AiInfo.TargetPlayer != nullptr &&
		AiInfo.bIsPlayerDead == false &&
		EnemyState == EEnemyState::EES_Chase;
}

void AEnemyRangedSiege::SiegeModeAttack()
{
	PlaySiegeModeFireMontage();
	SiegeModeProjectileFire();
}

void AEnemyRangedSiege::SiegeModeProjectileFire()
{
	const USkeletalMeshSocket* MuzzleSocket = GetMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) 
		return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetMesh());

	if (SiegeModeProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(SiegeModeProjectileClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), SpawnParams);
		}
	}
}

void AEnemyRangedSiege::ConvertSiegeMode()
{
	EnemyState = EEnemyState::EES_Siege;
	if (AIController)
	{
		AIController->ActivateSiegeMode();
	}
	Defense = 60.f;
	Damage = 700.f;
}

void AEnemyRangedSiege::ReleaseSiegeMode()
{
	SetEnemyState(EEnemyState::EES_Chase);
	if (AIController)
	{
		AIController->DeactivateSiegeMode();
	}
	Defense = 45.f;
	Damage = 400.f;
}

void AEnemyRangedSiege::PlaySiegeModeFireMontage()
{
	if (EnemyAnim == nullptr || SiegeModeFireMontage == nullptr) 
		return;

	bIsAttacking = true;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}

	int8 RandSectionNum = UKismetMathLibrary::RandomInteger(SiegeModeFireMontageSectionNameArr.Num());
	EnemyAnim->Montage_Play(SiegeModeFireMontage);
	EnemyAnim->Montage_JumpToSection(SiegeModeFireMontageSectionNameArr[RandSectionNum]);
}

void AEnemyRangedSiege::PlaySiegeModeHitReactionMontage()
{
	if (EnemyAnim == nullptr || SiegeModeHitReactionMontage == nullptr) 
		return;

	EnemyAnim->Montage_Play(SiegeModeHitReactionMontage);

	EnemyAnim->Montage_JumpToSection(FName("HitFront"));
}

void AEnemyRangedSiege::OnSiegeModeFireMontageEnded()
{
	bIsAttacking = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	OnAttackEnd.Broadcast();
}

void AEnemyRangedSiege::OnSiegeModeHitReactionMontageEnded()
{
	AiInfo.bStiffed = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
}
