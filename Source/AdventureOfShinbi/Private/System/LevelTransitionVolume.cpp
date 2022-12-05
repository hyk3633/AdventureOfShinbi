
#include "System/LevelTransitionVolume.h"
#include "System/AOSGameInstance.h"
#include "System/AOSGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Player/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

ALevelTransitionVolume::ALevelTransitionVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionObjectType(ECC_WorldStatic);
	StaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComp->SetupAttachment(RootComponent);
	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionObjectType(ECC_ItemRange);
	BoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);

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
	GetWorld()->GetGameInstance<UAOSGameInstance>()->SavePlayerData();
	GetWorld()->GetGameInstance<UAOSGameInstance>()->AcitavateShouldLoadData();
	UGameplayStatics::OpenLevel(this, FName(LevelName));
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

