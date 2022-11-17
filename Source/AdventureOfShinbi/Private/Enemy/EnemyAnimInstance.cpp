

#include "Enemy/EnemyAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	EnemyCharacter = Cast<AEnemyCharacter>(TryGetPawnOwner());
}

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (EnemyCharacter == nullptr) 
		return;

	FVector Velocity = EnemyCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	if (EnemyCharacter->GetVelocity().Z > 0.f)
	{
		bIsFloating = true;
		bIsFalling = false;
	}
	else if (EnemyCharacter->GetVelocity().Z < 0.f)
	{
		bIsFloating = false;
		bIsFalling = true;
	}
	else
	{
		bIsFalling = false;
	}
	
	EnemyState = EnemyCharacter->GetEnemyState();

	bIsAttacking = EnemyCharacter->GetIsAttacking();
}