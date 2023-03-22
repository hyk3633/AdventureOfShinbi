

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
	// StrafingTime �ð� �� ȸ�� �⵿ ���� ����
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

	// ȸ�� �⵿ ������ �������� ������ �̵� �ӵ��� ����
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
	if (bDeath == false &&						// ĳ���Ͱ� ����ְ�
		bStrafing &&							// ȸ�� �⵿�� ���� ���̰�
		bIsAttacking == false &&				// ���� ���� �ƴϰ�
		AiInfo.bTargetInAttackRange == false &&	// Ÿ���� ���� ���� ���̰�
		AiInfo.bStunned == false &&				// ���� ���°� �ƴϰ�
		AiInfo.bStiffed == false &&				// ���� ���°� �ƴϰ�
		AiInfo.bIsKnockUp == false)				// ��� ���°� �ƴϸ� true
		return true;
	else
	{
		// �׷��� ������ ȸ�� �⵿ �ߴ�
		bStrafing = false;
		EndStrafing();
		return false;
	}
}

void AEnemyStrafing::ChangeStrafingDirection()
{
	// ������ ������ ȸ�� �⵿ ���� ����
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

	// 2~5 �� �� �ٸ� �������� ��ȯ
	StrafingTime = 2.f + FMath::RandRange(0.f, 3.f);

	GetWorldTimerManager().SetTimer(StrafingTimer, this, &AEnemyStrafing::ChangeStrafingDirection, StrafingTime);
}

FVector AEnemyStrafing::WorldDirectionForStrafing()
{
	// enum�� ���� ĳ���Ͱ� �̵��� ���� ��ȯ
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

