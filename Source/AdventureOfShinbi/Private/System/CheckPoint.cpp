

#include "System/CheckPoint.h"
#include "System/AOSGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Player/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Blueprint/UserWidget.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

ACheckPoint::ACheckPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(RootComponent);
	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionObjectType(ECC_ItemRange);
	BoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);

}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnBoxOverlap);
}

void ACheckPoint::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha)
	{
		if (CheckPointSignClass)
		{
			CheckPointSign = CreateWidget<UUserWidget>(GetWorld(), CheckPointSignClass);
			CheckPointSign->AddToViewport();
		}

		if (CheckSound)
		{
			UGameplayStatics::PlaySound2D(this, CheckSound);
		}

		GetWorld()->GetAuthGameMode<AAOSGameModeBase>()->SetPlayerStart(this);

		BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


