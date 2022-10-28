// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyRangedSiege.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/EnemyAIController.h"
#include "Player/AOSCharacter.h"
#include "Weapons/Projectile.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Engine/SkeletalMeshSocket.h"

AEnemyRangedSiege::AEnemyRangedSiege()
{

}

void AEnemyRangedSiege::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyRangedSiege::HandleStiffAndStun(FName& BoneName)
{
	if (BoneName == FName("head") && EnemyState != EEnemyState::EES_Siege)
	{
		float Chances = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		if (StunChance > Chances)
		{
			AiInfo.bStunned = true;
			PlayStunMontage();
		}
		else
		{
			if (EnemyState == EEnemyState::EES_Patrol)
			{
				SetEnemyState(EEnemyState::EES_Detected);
			}
		}
	}
	else
	{
		float Chances = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		if (StiffChance > Chances)
		{
			AiInfo.bStiffed = true;
			EnemyState == EEnemyState::EES_Siege ? PlaySiegeModeHitReactionMontage() : PlayHitReactionMontage();
		}
		else
		{
			if (EnemyState == EEnemyState::EES_Patrol)
			{
				SetEnemyState(EEnemyState::EES_Detected);
			}
		}
	}
}

void AEnemyRangedSiege::SiegeModeAttack()
{
	PlaySiegeModeFireMontage();
	SiegeModeProjectileFire();
}

void AEnemyRangedSiege::SiegeModeProjectileFire()
{
	const USkeletalMeshSocket* MuzzleSocket = GetMesh()->GetSocketByName("MuzzleSocket");
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetMesh());

	if (SiegeModeProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
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
	bSiegeMode = true;
	// ���� ����
}

void AEnemyRangedSiege::ReleaseSiegeMode()
{
	SetEnemyState(EEnemyState::EES_Comeback);
	bSiegeMode = false;
	// ���� ����
}

void AEnemyRangedSiege::PlaySiegeModeFireMontage()
{
	if (EnemyAnim == nullptr || SiegeModeFireMontage == nullptr) return;

	bIsAttacking = true;

	int8 RandSectionNum = UKismetMathLibrary::RandomInteger(SiegeModeFireMontageSectionNameArr.Num());

	EnemyAnim->Montage_Play(SiegeModeFireMontage);

	EnemyAnim->Montage_JumpToSection(SiegeModeFireMontageSectionNameArr[RandSectionNum]);
}

void AEnemyRangedSiege::PlaySiegeModeHitReactionMontage()
{
	if (EnemyAnim == nullptr || SiegeModeHitReactionMontage == nullptr) return;

	EnemyAnim->Montage_Play(SiegeModeHitReactionMontage);

	EnemyAnim->Montage_JumpToSection(FName("HitFront"));
}

void AEnemyRangedSiege::OnSiegeModeFireMontageEnded()
{
	bIsAttacking = false;
	OnAttackEnd.Broadcast();
}

void AEnemyRangedSiege::OnSiegeModeHitReactionMontageEnded()
{
	AiInfo.bStiffed = false;
}

bool AEnemyRangedSiege::GetSiegeMode() const
{
	return bSiegeMode;
}
