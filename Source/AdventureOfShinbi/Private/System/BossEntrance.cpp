

#include "System/BossEntrance.h"
#include "System/AOSGameModeBase.h"
#include "Components/BoxComponent.h"
#include "EnemyBoss/EnemyBoss.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

ABossEntrance::ABossEntrance()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComp->SetupAttachment(RootComponent);
	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionObjectType(ECC_ItemRange);
	BoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
}

void ABossEntrance::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetAuthGameMode<AAOSGameModeBase>()->DPlayerRespawn.AddUObject(this, &ABossEntrance::ResetEntrance);

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyBoss::StaticClass(), Actors);
	if (Actors.Num() > 0)
	{
		if (Actors[0])
		{
			Boss = Cast<AEnemyBoss>(Actors[0]);
		}
	}
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABossEntrance::OnBoxOverlap);
}

void ABossEntrance::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (bBind == false)
	{
		GetWorld()->GetAuthGameMode<AAOSGameModeBase>()->BindBossDefeatEvent();
		bBind = true;
	}

	// ÄÆ¾À Àç»ý

	StartBossStage();
}

void ABossEntrance::ResetEntrance()
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void ABossEntrance::StartBossStage()
{
	if (Boss)
	{
		Boss->SetTarget();
	}
}
