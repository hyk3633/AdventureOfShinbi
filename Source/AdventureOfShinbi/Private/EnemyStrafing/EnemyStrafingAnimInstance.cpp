// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStrafing/EnemyStrafingAnimInstance.h"
#include "EnemyStrafing/EnemyStrafing.h"

void UEnemyStrafingAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	EnemyStrafingCharacter = Cast<AEnemyStrafing>(TryGetPawnOwner());
}

void UEnemyStrafingAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	Super::UpdateAnimationProperties(DeltaTime);

	if (EnemyStrafingCharacter == nullptr) return;

	StrafingValue = EnemyStrafingCharacter->GetStrafingValue();

	JogAnimRate = EnemyStrafingCharacter->GetJogAnimRate();
}
