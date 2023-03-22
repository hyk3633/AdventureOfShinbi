

#include "EnemyMuriel/EnemyMuriel.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Player/AOSCharacter.h"
#include "System/AOSGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/CollisionProfile.h"
#include "DrawDebugHelpers.h"

AEnemyMuriel::AEnemyMuriel()
{
	PrimaryActorTick.bCanEverTick = true;

	AttackingRotateRate = 100.f;
	NormalRotateRate = 100.f;

	SightStimulusExpireTime = 0.f;

	Damage = 250.f;
	Health = 3000.f;
	MaxHealth = 3000.f;
	Defense = 50.f;
	DefaultValue = 3.f;
	RandRangeValue = 20;
}

void AEnemyMuriel::BeginPlay()
{
	if (bWhite)
	{
		FriendlyTag = FName("White");
		AssetName = TEXT("Muriel_GDC");
	}
	else
	{
		FriendlyTag = FName("Black");
		AssetName = TEXT("MurielBlack");
	}

	Super::BeginPlay();
}

void AEnemyMuriel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyMuriel::HandleStiffAndStun(bool IsHeadShot)
{
	if (bIsAttacking) 
		return;

	Super::HandleStiffAndStun(IsHeadShot);
}

void AEnemyMuriel::ResetAIState()
{
	Super::ResetAIState();

	GetWorldTimerManager().ClearTimer(BuffCoolTimer);
	GetWorldTimerManager().ClearTimer(SummonCoolTimer);
	GetWorldTimerManager().ClearTimer(TeleportCoolTimer);
	GetWorldTimerManager().ClearTimer(SkillShotCoolTimer);
	bBuffCoolTimeEnd = true;
	bSummonCoolTimeEnd = true;
	bSkillShotCoolTimeEnd = true;
	bTeleportMinionCoolTimeEnd = true;
}

void AEnemyMuriel::HandleHealthChange(float DamageReceived)
{
	if (Health == 0)
	{
		EnemyAnim->Montage_Stop(0.f, FireMontage);
		DMurielDeath.Broadcast();
	}

	Super::HandleHealthChange(DamageReceived);
}

void AEnemyMuriel::RangedAttack()
{
	if (bSkillShotCoolTimeEnd && FMath::RandRange(0.f, 1.f) <= 0.3f)
	{
		FireSkillShot();
	}
	else
	{
		Super::RangedAttack();
	}
}

void AEnemyMuriel::SummonMinion()
{
	if (EnemyClassArr.Num() == 0)
		return;

	bSummonCoolTimeEnd = false;
	bIsAttacking = true;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	PlaySummonMontage();
}

void AEnemyMuriel::Summon()
{
	int8 SummonSuccess = 0;

	// 총 2마리 미니언 소환
	for (int8 i = 0; i < 2; i++)
	{
		// 소환 할 위치에 다른 액터가 있으면 패스
		FVector SummonPosition;
		const bool bSummonIsSafe = CheckSpawnPosition(SummonPosition, this, 150.f);
		if (bSummonIsSafe == false)
			continue;

		// 80% 확률로 기본 미니언, 레인저 미니언 중 하나를 소환
		const int8 RandIdx = FMath::RandRange(0.f, 1.f) <= 0.8f ? FMath::RandRange(0, 1) : FMath::RandRange(2, 3);

		if (EnemyClassArr[RandIdx])
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = this;

			UWorld* World = GetWorld();
			if (World)
			{
				PlaySummonParticle(SummonPosition);
				World->SpawnActor<AEnemyCharacter>(EnemyClassArr[RandIdx], SummonPosition, GetActorRotation(), SpawnParams);
				SummonSuccess++;
			}
		}
	}

	// 하나라도 소환에 성공하면 쿨타임 카운트
	if (SummonSuccess > 0)
	{
		GetWorldTimerManager().SetTimer(SummonCoolTimer, this, &AEnemyMuriel::SummonCoolTimeEnd, SummonCoolTime);
	}
	else
	{
		bSummonCoolTimeEnd = true;
		bIsAttacking = false;
		if (AIController)
		{
			AIController->UpdateAiInfo();
		}
		OnAttackEnd.Broadcast();
	}
}

bool AEnemyMuriel::CheckSpawnPosition(FVector& SafePosition, AActor* CenterActor, float PositionOffset)
{
	UCollisionProfile* Profile = UCollisionProfile::Get();
	ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECollisionChannel::ECC_Visibility);
	
	// CenterActor의 위치를 중심으로 8 방향에서 소환 가능한 위치를 SafePosition에 저장
	float Angle = 0.f;
	for (int8 i = 0; i < 8; i++)
	{
		FHitResult BoxHit;

		const FVector BoxTraceStart = 
			CenterActor->GetActorLocation() + FVector(PositionOffset, 0.f, 0.f).RotateAngleAxis(Angle, FVector(0, 0, 1));
		const FVector BoxTraceEnd = BoxTraceStart + FVector(0.f, 0.f, 150.f);

		UKismetSystemLibrary::BoxTraceSingle
		(
			this,
			BoxTraceStart,
			BoxTraceEnd,
			FVector(70.f, 70.f, 0.f),
			CenterActor->GetActorRotation(),
			TraceType,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			BoxHit,
			true
		);

		if (BoxHit.bBlockingHit == false)
		{
			SafePosition = BoxTraceStart;
			RotationIdx = i;

			return true;
		}

		Angle += 45.f;
	}

	return false;
}

void AEnemyMuriel::PlaySummonMontage()
{
	if (EnemyAnim == nullptr || SummonMontage == nullptr) 
		return;

	EnemyAnim->Montage_Play(SummonMontage);
}

void AEnemyMuriel::PlaySummonParticle(FVector Position)
{
	if (SummonParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SummonParticle, Position);
	}
}

void AEnemyMuriel::OnSummonMontageEnded()
{
	bIsAttacking = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	OnAttackEnd.Broadcast();
}

void AEnemyMuriel::SummonCoolTimeEnd()
{
	bSummonCoolTimeEnd = true;
}

void AEnemyMuriel::ProvideBuff()
{
	bBuffCoolTimeEnd = false;
	bIsAttacking = true;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	PlayBuffMontage();
}

void AEnemyMuriel::Buff()
{
	TArray<AActor*> TargetsToBuff;
	// 태그가 지정된 적 클래스 검색
	UGameplayStatics::GetAllActorsOfClassWithTag(this, AEnemyCharacter::StaticClass(), FriendlyTag, TargetsToBuff);
	// 검색 결과가 없으면 태스크 종료
	if (TargetsToBuff.Num() == 0)
	{
		bBuffCoolTimeEnd = true;
		bIsAttacking = false;
		if (AIController)
		{
			AIController->UpdateAiInfo();
		}
		OnAttackEnd.Broadcast();
		return;
	}

	for (AActor* const& TaggedActor : TargetsToBuff)
	{
		if (IsValid(TaggedActor) == false)
			continue;

		AEnemyCharacter* Friendly = Cast<AEnemyCharacter>(TaggedActor);
		if (Friendly)
		{
			// 거리가 1000 이내인 미니언에게만 버프 가동 
			if (GetDistanceTo(Friendly) <= 1000.f)
			{
				if (bWhite)
				{
					Friendly->ActivateHealing(Friendly->GetMaxHealth() * 0.5);
				}
				else
				{
					Friendly->ActivateDamageUp(1.25f);
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(BuffCoolTimer, this, &AEnemyMuriel::BuffCoolTimeEnd, BuffCoolTime);
}

void AEnemyMuriel::PlayBuffMontage()
{
	if (EnemyAnim == nullptr || BuffMontage == nullptr) 
		return;

	EnemyAnim->Montage_Play(BuffMontage);
}

void AEnemyMuriel::OnBuffMontageEnded()
{
	bIsAttacking = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	OnAttackEnd.Broadcast();
}

void AEnemyMuriel::BuffCoolTimeEnd()
{
	bBuffCoolTimeEnd = true;
}

void AEnemyMuriel::FireSkillShot()
{
	bSkillShotCoolTimeEnd = false;
	bIsAttacking = true;
	Damage = 400.f;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	PlaySkillShotFireMontage();
	GetWorldTimerManager().SetTimer(SkillShotCoolTimer, this, &AEnemyMuriel::SkillShotCoolTimeEnd, SkillShotCoolTime);
}

void AEnemyMuriel::PlaySkillShotFireMontage()
{
	if (EnemyAnim == nullptr || SkillShotFireMontage == nullptr) 
		return;

	EnemyAnim->Montage_Play(SkillShotFireMontage);
}

void AEnemyMuriel::FireSkillShotProjectile()
{
	ProjectileFire(SkillProjectileClass);
}

void AEnemyMuriel::OnSkillShotFireMontageEnded()
{
	bIsAttacking = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	Damage = 250.f;
	OnAttackEnd.Broadcast();
}

void AEnemyMuriel::SkillShotCoolTimeEnd()
{
	bSkillShotCoolTimeEnd = true;
}

void AEnemyMuriel::FindTeleportPosition()
{
	if (
		IsValid(AiInfo.TargetPlayer) == false ||                                    // 타겟이 유효하지 않거나
		CheckSpawnPosition(TeleportPosition, AiInfo.TargetPlayer, 200.f) == false   // 스폰 위치에 장애물이 있을 경우 실패
		)
	{
		SkillFailed();
		return;
	}

	AAOSCharacter* AC = Cast<AAOSCharacter>(AiInfo.TargetPlayer);
	if (AC)
	{
		TArray<AActor*> Allies;
		UGameplayStatics::GetAllActorsWithTag(this, FriendlyTag, Allies);
		if (Allies.Num() == 0)
		{
			SkillFailed();
			return;
		}

		// 타겟 플레이어에서 가장 멀리 떨어진 미니언을 탐색
		float FarestDistance = 0.f;
		FarestMinion = nullptr;
		for (AActor* const& TaggedActor : Allies)
		{
			if (IsValid(TaggedActor) == false)
				continue;

			if (FarestDistance < TaggedActor->GetDistanceTo(AC))
			{
				FarestDistance = TaggedActor->GetDistanceTo(AC);
				FarestMinion = TaggedActor;
			}
		}

		if (FarestMinion == nullptr)
		{
			SkillFailed();
			return;
		}

		// 이동할 미니언이 타겟 플레이어를 마주 보도록 Rotation 설정
		TeleportRotation = UKismetMathLibrary::FindLookAtRotation(TeleportPosition, AC->GetActorLocation());

		bTeleportMinionCoolTimeEnd = false;
		bIsAttacking = true;
		if (AIController)
		{
			AIController->UpdateAiInfo();
		}

		PlayTeleportMinionMontage();
		TeleportMinionToPlayer();

		GetWorldTimerManager().SetTimer(TeleportCoolTimer, this, &AEnemyMuriel::TeleportMinionCoolTimeEnd, TeleportCoolTime);
	}
}

void AEnemyMuriel::TeleportMinionToPlayer()
{
	PlayTeleportMinionEffect();
	AEnemyCharacter* EC = Cast<AEnemyCharacter>(FarestMinion);
	EC->GetMesh()->SetVisibility(false);

	GetWorldTimerManager().SetTimer(TeleportTimer, this, &AEnemyMuriel::TeleportMinionDelayEnd, TeleportDelayTime);
}

void AEnemyMuriel::PlayTeleportMinionEffect()
{
	if (TeleportMinionParticle1)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportMinionParticle1, FarestMinion->GetActorLocation(), FarestMinion->GetActorRotation());
	}
	if (TeleportMinionParticle2)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportMinionParticle2, FarestMinion->GetActorLocation(), FarestMinion->GetActorRotation());
	}
	if (TeleportSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, TeleportSound, FarestMinion->GetActorLocation());
	}
}

void AEnemyMuriel::PlayTeleportMinionMontage()
{
	if (EnemyAnim == nullptr || TeleportMinionMontage == nullptr) 
		return;

	EnemyAnim->Montage_Play(TeleportMinionMontage);
}

void AEnemyMuriel::OnTeleportMinionMontageEnded()
{
	bIsAttacking = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	OnAttackEnd.Broadcast();
}

void AEnemyMuriel::TeleportMinionDelayEnd()
{
	AEnemyCharacter* EC = Cast<AEnemyCharacter>(FarestMinion);
	if (EC == nullptr) 
		return;
	
	FarestMinion->SetActorLocation(TeleportPosition);
	FarestMinion->SetActorRotation(TeleportRotation);
	EC->GetMesh()->SetVisibility(true);
	PlayTeleportMinionEffect();
}

void AEnemyMuriel::TeleportMinionCoolTimeEnd()
{
	bTeleportMinionCoolTimeEnd = true;
}

void AEnemyMuriel::SkillFailed()
{
	bIsAttacking = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	OnAttackEnd.Broadcast();
}

bool AEnemyMuriel::GetMurielType() const
{
	return bWhite;
}

float AEnemyMuriel::GetSkillShotDurationTime() const
{
	return SkillShotDurationTime;
}

bool AEnemyMuriel::GetBuffCoolTimeEnd() const
{
	return bBuffCoolTimeEnd;
}

bool AEnemyMuriel::GetSummonCoolTimeEnd() const
{
	return bSummonCoolTimeEnd;
}

FName AEnemyMuriel::GetFriendlyTag() const
{
	return FriendlyTag;
}

bool AEnemyMuriel::GetTeleportMinionCoolTimeEnd() const
{
	return bTeleportMinionCoolTimeEnd;
}