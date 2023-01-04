
#include "System/LevelTransitionVolume.h"
#include "System/AOSGameInstance.h"
#include "System/AOSGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

ALevelTransitionVolume::ALevelTransitionVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(BoxComp);
	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionObjectType(ECC_ItemRange);
	BoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);

	MarkParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Mark Particle"));
	MarkParticle->SetupAttachment(RootComponent);

}

void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionVolume::OnBoxOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &ALevelTransitionVolume::OnBoxOverlapEnd);

	GetWorld()->GetAuthGameMode<AAOSGameModeBase>()->DPlayerRespawn.AddUObject(this, &ALevelTransitionVolume::ReBindFunction);

	AAOSCharacter* Cha = Cast<AAOSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Cha)
	{
		Cha->DLevelTransition.BindUObject(this, &ALevelTransitionVolume::LevelTransition);
	}
}

void ALevelTransitionVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha)
	{
		Cha->SetOverlappedLTV(true);
		ShowLevelTrasitionSign();
	}
}

void ALevelTransitionVolume::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha)
	{
		Cha->SetOverlappedLTV(false);
		if (LevelTrasnsitionSign)
		{
			LevelTrasnsitionSign->Destruct();
		}
	}
}

void ALevelTransitionVolume::ReBindFunction()
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Cha)
	{
		Cha->DLevelTransition.BindUObject(this, &ALevelTransitionVolume::LevelTransition);
	}
}

void ALevelTransitionVolume::LevelTransition()
{
	// 데이터 저장
	GetWorld()->GetGameInstance<UAOSGameInstance>()->SavePlayerData();
	GetWorld()->GetGameInstance<UAOSGameInstance>()->AcitavateShouldLoadData();
	if (LoadingScreen)
	{
		UUserWidget* LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreen);
		LoadingScreenWidget->AddToViewport();
		GetWorldTimerManager().SetTimer(TransitionTimer, this, &ALevelTransitionVolume::OpenNextLevel, 0.3f);
	}
}

void ALevelTransitionVolume::ShowLevelTrasitionSign()
{
	if (LevelTrasnsitionSignClass)
	{
		LevelTrasnsitionSign = CreateWidget<UUserWidget>(GetWorld(), LevelTrasnsitionSignClass);
		if (LevelTrasnsitionSign)
		{
			LevelTrasnsitionSign->AddToViewport();
		}
	}
}

void ALevelTransitionVolume::OpenNextLevel()
{
	UGameplayStatics::OpenLevel(this, FName(LevelName));
}

