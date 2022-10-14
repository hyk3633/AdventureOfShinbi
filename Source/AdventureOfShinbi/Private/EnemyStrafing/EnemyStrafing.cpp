

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
	UE_LOG(LogTemp, Warning, TEXT("End"));
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
	if (
		!bDeath &&
		bStrafing && 
		!bIsAttacking && 
		StrafingDir != EStrafingDirection::ESD_Stop && 
		!AIController->GetBlackBoard()->GetValueAsBool(FName("TargetInAttackRange")) &&
		!AIController->GetBlackBoard()->GetValueAsBool(FName("Stunned")) &&
		!AIController->GetBlackBoard()->GetValueAsBool(FName("Stiffed")) &&
		!AIController->GetBlackBoard()->GetValueAsBool(FName("KnockUp"))
		)
	{
		const AActor* Target = Cast<AActor>(AIController->GetBlackBoard()->GetValueAsObject(FName("Target")));
		if (Target)
		{
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
			SetActorRotation(LookAtRotation);
		}

		const FVector WorldDirection = WorldDirectionForStrafing();

		AddMovementInput(WorldDirection);

		StrafingValue = StrafingDir == EStrafingDirection::ESD_Back ? -180.f : UKismetMathLibrary::NormalizeAxis((WorldDirection.Rotation() - GetControlRotation()).Yaw);

		JogAnimRate = 0.6f;
		GetCharacterMovement()->MaxWalkSpeed = 150.f;
	}
	else
	{
		StrafingValue = 0.f;
		JogAnimRate = 1.f;
		GetCharacterMovement()->MaxWalkSpeed = 450.f;
	}

	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green, FString::Printf(TEXT("%f"), StrafingValue));
}

void AEnemyStrafing::ChangeStrafingDirection()
{
	const int32 RandDir = FMath::RandRange(0, 6);

	switch (RandDir)
	{
	case 0:
		StrafingDir = EStrafingDirection::ESD_Front;
		StrafingTime = 1.f;
		break;
	case 1:
		StrafingDir = EStrafingDirection::ESD_Left;
		StrafingTime = 2.f;
		break;
	case 2:
		StrafingDir = EStrafingDirection::ESD_Right;
		StrafingTime = 3.f;
		break;
	case 3:
		StrafingDir = EStrafingDirection::ESD_Back;
		StrafingTime = 4.f;
		break;
	case 4:
		StrafingDir = EStrafingDirection::ESD_RDiagonal;
		StrafingTime = 5.f;
		break;
	case 5:
		StrafingDir = EStrafingDirection::ESD_LDiagonal;
		StrafingTime = 6.f;
		break;
	case 6:
		StrafingDir = EStrafingDirection::ESD_Stop;
		StrafingTime = 7.f;
		break;
	}

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
			AIController->GetBlackBoard()->SetValueAsBool(FName("KnockUp"), true);
		}
	}
}

void AEnemyStrafing::KnockUpEnd()
{
	if (AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("KnockUp"), false);
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

