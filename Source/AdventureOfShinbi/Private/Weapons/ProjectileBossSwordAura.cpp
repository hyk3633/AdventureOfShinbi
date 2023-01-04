

#include "Weapons/ProjectileBossSwordAura.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"
#include "Particles/ParticleSystemComponent.h"

AProjectileBossSwordAura::AProjectileBossSwordAura()
{

}

void AProjectileBossSwordAura::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	Target = Cast<AAOSCharacter>(OtherActor);
	if (Target)
	{
		Target->SetWalkingSpeed(EWalkingState::EWS_Slowed);
		GetWorldTimerManager().SetTimer(SlowTimer, this, &AProjectileBossSwordAura::SlowTimeEnd, SlowTime);
	}
}

void AProjectileBossSwordAura::SlowTimeEnd()
{
	Target->SetWalkingSpeed(EWalkingState::EWS_Armed);
}
