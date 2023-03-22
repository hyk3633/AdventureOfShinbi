

#include "EnemyStrafing/EnemyStrafing.h"
#include "EnemyStrafing/EnemyStrafingAnimInstance.h"
#include "Enemy/EnemyAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"

AEnemyStrafing::AEnemyStrafing()
{
	BoxTraceSize = FVector(10.f,10.f,10.f);

	PatrolSpeed = 250.f;
	ChaseSpeed = 450.f;

	Damage = 350.f;
	Health = 2000.f;
	MaxHealth = 2000.f;
	Defense = 40.f;
	DefaultValue = 3.f;
	RandRangeValue = 50;

	AssetName = TEXT("Aurora_2");
}

void AEnemyStrafing::StartStrafing()
{
	// StrafingTime 시간 후 회피 기동 방향 변경
	bStrafing = true;
	GetWorldTimerManager().SetTimer(StrafingTimer, this, &AEnemyStrafing::ChangeStrafingDirection, StrafingTime);
}

void AEnemyStrafing::EndStrafing()
{
	bStrafing = false;
	GetWorldTimerManager().ClearTimer(StrafingTimer);
	OnStrafingEnd.Broadcast();
}

void AEnemyStrafing::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyStrafing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoStrafing();
}

void AEnemyStrafing::ResetAIState()
{
	Super::ResetAIState();

	bStrafing = false;
	EndStrafing();
}

void AEnemyStrafing::DoStrafing()
{
	if (bStrafing == false)
		return;

	// 회피 기동 조건을 만족하지 않으면 이동 속도를 변경
	const bool StrafingCondition = CheckStrafingCondition();
	if (StrafingCondition)
	{
		SetStrafingValue();

		JogAnimRate = StrafingAnimRate;
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	}
	else
	{
		StrafingValue = 0.f;
		JogAnimRate = ChaseAnimRate;
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	}
}

void AEnemyStrafing::SetStrafingValue()
{
	const AActor* Target = Cast<AActor>(AiInfo.TargetPlayer);
	if (Target)
	{
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
		SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
	}

	const FVector WorldDirection = WorldDirectionForStrafing();

	AddMovementInput(WorldDirection);

	StrafingValue = UKismetMathLibrary::NormalizeAxis((WorldDirection.Rotation() - GetControlRotation()).Yaw);
}

bool AEnemyStrafing::CheckStrafingCondition()
{
	if (bDeath == false &&						// 캐릭터가 살아있고
		bStrafing &&							// 회피 기동이 수행 중이고
		bIsAttacking == false &&				// 공격 중이 아니고
		AiInfo.bTargetInAttackRange == false &&	// 타겟이 공격 범위 밖이고
		AiInfo.bStunned == false &&				// 스턴 상태가 아니고
		AiInfo.bStiffed == false &&				// 경직 상태가 아니고
		AiInfo.bIsKnockUp == false)				// 녹업 상태가 아니면 true
		return true;
	else
	{
		// 그렇지 않으면 회피 기동 중단
		bStrafing = false;
		EndStrafing();
		return false;
	}
}

void AEnemyStrafing::ChangeStrafingDirection()
{
	// 무작위 값으로 회피 기동 방향 결정
	const int32 RandDir = FMath::RandRange(0, 4);

	switch (RandDir)
	{
	case 0:
		StrafingDir = EStrafingDirection::ESD_Front;
		break;
	case 1:
		StrafingDir = EStrafingDirection::ESD_Left;
		break;
	case 2:
		StrafingDir = EStrafingDirection::ESD_Right;
		break;
	case 3:
		StrafingDir = EStrafingDirection::ESD_RDiagonal;
		break;
	case 4:
		StrafingDir = EStrafingDirection::ESD_LDiagonal;
		break;
	}

	// 2~5 초 후 다른 방향으로 전환
	StrafingTime = 2.f + FMath::RandRange(0.f, 3.f);

	GetWorldTimerManager().SetTimer(StrafingTimer, this, &AEnemyStrafing::ChangeStrafingDirection, StrafingTime);
}

FVector AEnemyStrafing::WorldDirectionForStrafing()
{
	// enum에 따라 캐릭터가 이동할 방향 반환
	switch (StrafingDir)
	{
	case EStrafingDirection::ESD_Front:
		return GetActorForwardVector();

	case EStrafingDirection::ESD_Left:
		return GetActorRightVector() * -1.f;

	case EStrafingDirection::ESD_Right:
		return GetActorRightVector();

	case EStrafingDirection::ESD_RDiagonal:
		return GetActorRightVector() + GetActorForwardVector();

	case EStrafingDirection::ESD_LDiagonal:
		return (GetActorRightVector() * -1.f) + GetActorForwardVector();
	}

	return GetActorForwardVector();
}

void AEnemyStrafing::KnockUpEnd()
{
	AiInfo.bIsKnockUp = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
}

float AEnemyStrafing::GetStrafingValue() const
{
	return StrafingValue;
}

float AEnemyStrafing::GetJogAnimRate() const
{
	return JogAnimRate;
}

bool AEnemyStrafing::GetStrafing() const
{
	return bStrafing;
}

