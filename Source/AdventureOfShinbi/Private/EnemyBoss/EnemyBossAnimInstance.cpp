// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss/EnemyBossAnimInstance.h"
#include "EnemyBoss/EnemyBoss.h"

void UEnemyBossAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Boss = Cast<AEnemyBoss>(TryGetPawnOwner());
}

void UEnemyBossAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	Super::UpdateAnimationProperties(DeltaTime);

	if (Boss == nullptr) return;

	bPhase2 = Boss->GetPhase2();

	bStrafing = Boss->GetStrafing();
}
