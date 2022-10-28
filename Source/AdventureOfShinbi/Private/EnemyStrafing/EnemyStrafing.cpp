

#include "EnemyStrafing/EnemyStrafing.h"
#include "EnemyStrafing/EnemyStrafingAnimInstance.h"
#include "Enemy/EnemyAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

AEnemyStrafing::AEnemyStrafing()
{
	
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

	DrawDebugSphere(GetWorld(), GetActorLocation(), 1000.f, 12, FColor::Blue, false, -1.0f, 0U, 2.f);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 300.f, 12, FColor::Green, false, -1.0f, 0U, 2.f);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 900.f, 12, FColor::Red, false, -1.0f, 0U, 2.f);

	DoStrafing();
}

void AEnemyStrafing::DoStrafing()
{
	const bool StrafingCondition = CheckStrafingCondition();
	if (StrafingCondition)
	{
		const AActor* Target = Cast<AActor>(AiInfo.TargetPlayer);
		if (Target)
		{
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
			SetActorRotation(LookAtRotation);
		}

		const FVector WorldDirection = WorldDirectionForStrafing();

		AddMovementInput(WorldDirection);

		StrafingValue = StrafingDir == EStrafingDirection::ESD_Back ? -180.f : UKismetMathLibrary::NormalizeAxis((WorldDirection.Rotation() - GetControlRotation()).Yaw);

		JogAnimRate = StrafingAnimRate;
		GetCharacterMovement()->MaxWalkSpeed = StrafingSpeed;
	}
	else
	{
		StrafingValue = 0.f;
		JogAnimRate = ChaseAnimRate;
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	}
}

bool AEnemyStrafing::CheckStrafingCondition()
{
	return bDeath == false &&
		bStrafing &&
		bIsAttacking == false &&
		StrafingDir != EStrafingDirection::ESD_Stop &&
		AiInfo.bTargetInAttackRange == false &&
		AiInfo.bStunned == false &&
		AiInfo.bStiffed == false &&
		AiInfo.bIsKnockUp == false;
}

void AEnemyStrafing::ChangeStrafingDirection()
{
	const int32 RandDir = FMath::RandRange(0, 6);

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
		StrafingDir = EStrafingDirection::ESD_Back;
		break;
	case 4:
		StrafingDir = EStrafingDirection::ESD_RDiagonal;
		break;
	case 5:
		StrafingDir = EStrafingDirection::ESD_LDiagonal;
		break;
	case 6:
		StrafingDir = EStrafingDirection::ESD_Stop;
		break;
	}

	StrafingTime = 3.f + FMath::RandRange(1.f, 5.f);

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

	case EStrafingDirection::ESD_Back:
		return GetActorForwardVector() * -1.f;

	case EStrafingDirection::ESD_RDiagonal:
		return GetActorRightVector() + GetActorForwardVector();

	case EStrafingDirection::ESD_LDiagonal:
		return (GetActorRightVector() * -1.f) + GetActorForwardVector();
	}

	return GetActorForwardVector();
}

void AEnemyStrafing::CheckIsKnockUp()
{
	if (AIController)
	{
		if (GetCharacterMovement()->IsFalling())
		{
			EnemyAnim->StopAllMontages(0.2f);
			AiInfo.bIsKnockUp = true;
		}
	}
}

void AEnemyStrafing::KnockUpEnd()
{
	AiInfo.bIsKnockUp = false;
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

