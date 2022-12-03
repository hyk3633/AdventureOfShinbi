

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
}

void AEnemyStrafing::StartStrafing()
{
	bStrafing = true;
	GetWorldTimerManager().SetTimer(StrafingTimer, this, &AEnemyStrafing::ChangeStrafingDirection, StrafingTime);
}

void AEnemyStrafing::EndStrafing()
{
	bStrafing = false;
	GetWorldTimerManager().ClearTimer(StrafingTimer);
}

void AEnemyStrafing::BeginPlay()
{
	Super::BeginPlay();

	OnStrafingEnd.AddUObject(this, &AEnemyStrafing::EndStrafing);
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
}

void AEnemyStrafing::DoStrafing()
{
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
		SetActorRotation(LookAtRotation);
	}

	const FVector WorldDirection = WorldDirectionForStrafing();

	AddMovementInput(WorldDirection);

	StrafingValue = UKismetMathLibrary::NormalizeAxis((WorldDirection.Rotation() - GetControlRotation()).Yaw);
}

bool AEnemyStrafing::CheckStrafingCondition()
{
	return bDeath == false &&
		bStrafing &&
		bIsAttacking == false &&
		AiInfo.bTargetInAttackRange == false &&
		AiInfo.bStunned == false &&
		AiInfo.bStiffed == false &&
		AiInfo.bIsKnockUp == false;
}

void AEnemyStrafing::ChangeStrafingDirection()
{
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

	StrafingTime = 2.f + FMath::RandRange(0.f, 3.f);

	GetWorldTimerManager().SetTimer(StrafingTimer, this, &AEnemyStrafing::ChangeStrafingDirection, StrafingTime);
}

FVector AEnemyStrafing::WorldDirectionForStrafing()
{
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

