

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
			// �¸����� Ÿ�Կ� ���� 
			if (EM->GetMurielType())
			{
				// �÷��̾��� ü�� ȸ�� ����
				HittedPlayer->GetCombatComp()->HealBan(EM->GetSkillShotDurationTime());
			}
			else
			{
				// �÷��̾��� ���ݷ� ����
				HittedPlayer->GetCombatComp()->DecreaseDamage(EM->GetSkillShotDurationTime());
			}
		}
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
