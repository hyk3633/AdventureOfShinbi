

#include "System/BossEntrance.h"
#include "System/AOSGameModeBase.h"
#include "Components/BoxComponent.h"
#include "EnemyBoss/EnemyBoss.h"
#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "Components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include <LevelSequencePlayer.h>
#include <LevelSequenceActor.h>
#include <LevelSequence.h>
#include "MovieSceneSequencePlayer.h"
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

	BlockingVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Blocking Volume"));
	BlockingVolume->SetupAttachment(RootComponent);
	BlockingVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlockingVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	BlockingVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ABossEntrance::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetAuthGameMode<AAOSGameModeBase>()->DPlayerRespawn.AddUObject(this, &ABossEntrance::ResetEntrance);

	if (BossActor)
	{
		Boss = Cast<AEnemyBoss>(BossActor);
	}
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABossEntrance::OnBoxOverlap);

	ALevelSequenceActor* SequenceActor;
	FMovieSceneSequencePlaybackSettings Settings;
	if (SequenceAsset && SequencePlayer == nullptr)
	{
		SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(this, SequenceAsset, Settings, SequenceActor);
		if (SequencePlayer)
		{
			SequencePlayer->OnPlay.AddDynamic(this, &ABossEntrance::OnLevelSequencePlayed);
			SequencePlayer->OnFinished.AddDynamic(this, &ABossEntrance::OnLevelSequenceFinished);
		}
	}

	if (BossActor)
	{
		BossActor->SetActorHiddenInGame(true);
	}
	if (SequenceParticle)
	{
		SequenceParticle->SetActorHiddenInGame(true);
	}
	if (SequenceSkeletalMesh)
	{
		SequenceSkeletalMesh->SetActorHiddenInGame(true);
	}
}

void ABossEntrance::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Player = Cast<AAOSCharacter>(OtherActor);

	if (bBind == false)
	{
		// 보스 패배 델리게이트 바인딩
		GetWorld()->GetAuthGameMode<AAOSGameModeBase>()->BindBossDefeatEvent();
		bBind = true;
	}

	if (bSequencePlayed == false)
	{
		if (SequencePlayer)
		{
			SequencePlayer->Play();
			bSequencePlayed = true;
		}
	}
	else
	{
		BlockingVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BlockingVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

		StartBossStage();
	}
}

void ABossEntrance::ResetEntrance()
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);

	BlockingVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlockingVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (SequenceParticle)
	{
		SequenceParticle->SetActorHiddenInGame(true);
	}
	if (SequenceSkeletalMesh)
	{
		SequenceSkeletalMesh->SetActorHiddenInGame(true);
	}
}

void ABossEntrance::StartBossStage()
{
	if (Boss)
	{
		Boss->SetTarget();
	}
}

void ABossEntrance::OnLevelSequencePlayed()
{
	if (SequenceParticle)
	{
		SequenceParticle->SetActorHiddenInGame(false);
	}
	if (SequenceSkeletalMesh)
	{
		SequenceSkeletalMesh->SetActorHiddenInGame(false);
	}
	
	if (Player)
	{
		Player->DisableInput(UGameplayStatics::GetPlayerController(this, 0));
	}
	AAOSController* Controller = Cast<AAOSController>(UGameplayStatics::GetPlayerController(this, 0));
	if (Controller)
	{
		Controller->HideHUD();
	}
}

void ABossEntrance::OnLevelSequenceFinished()
{
	if (BossActor)
	{
		BossActor->SetActorHiddenInGame(false);
	}
	if (SequenceParticle)
	{
		SequenceParticle->SetActorHiddenInGame(true);
	}
	if (SequenceSkeletalMesh)
	{
		SequenceSkeletalMesh->SetActorHiddenInGame(true);
	}

	if (Player)
	{
		Player->SetActorLocation(FVector(-200.f, 4550.f, 8141.f));
	}

	BlockingVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BlockingVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	StartBossStage();

	if (Player)
	{
		Player->EnableInput(UGameplayStatics::GetPlayerController(this, 0));
	}
	AAOSController* Controller = Cast<AAOSController>(UGameplayStatics::GetPlayerController(this, 0));
	if (Controller)
	{
		Controller->ShowHUD();
	}
}
