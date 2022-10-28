// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileBossSwordAura.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"
#include "Particles/ParticleSystemComponent.h"

AProjectileBossSwordAura::AProjectileBossSwordAura()
{
	LifeSpan = 15.f;
}

void AProjectileBossSwordAura::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AProjectile::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	BodyParticleComponent->Deactivate();

	PlayHitEffect(Hit, OtherActor);

	Target = Cast< AAOSCharacter>(OtherActor);
	if (Target)
	{
		Target->SetWalkingSpeed(EWalkingState::EWS_Slowed);
		GetWorldTimerManager().SetTimer(SlowTimer, this, &AProjectileBossSwordAura::SlowTimeEnd, SlowTime);
	}
}

void AProjectileBossSwordAura::SlowTimeEnd()
{
	Target->GetCombatComp()->GetEquippedWeapon() ? 
		Target->SetWalkingSpeed(EWalkingState::EWS_Armed) : Target->SetWalkingSpeed(EWalkingState::EWS_UnArmed);
}
