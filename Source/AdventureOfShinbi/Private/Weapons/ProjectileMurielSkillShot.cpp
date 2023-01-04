

#include "Weapons/ProjectileMurielSkillShot.h"
#include "EnemyMuriel/EnemyMuriel.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"

AProjectileMurielSkillShot::AProjectileMurielSkillShot()
{
}

void AProjectileMurielSkillShot::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileMurielSkillShot::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AEnemyMuriel* EM = Cast<AEnemyMuriel>(GetInstigator());
	if (EM)
	{
		AAOSCharacter* HittedPlayer = Cast<AAOSCharacter>(OtherActor);
		if (HittedPlayer)
		{
			// 뮤리엘의 타입에 따라 
			if (EM->GetMurielType())
			{
				// 플레이어의 체력 회복 차단
				HittedPlayer->GetCombatComp()->HealBan(EM->GetSkillShotDurationTime());
			}
			else
			{
				// 플레이어의 공격력 감소
				HittedPlayer->GetCombatComp()->DecreaseDamage(EM->GetSkillShotDurationTime());
			}
		}
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
