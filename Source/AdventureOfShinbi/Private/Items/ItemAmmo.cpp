

#include "Items/ItemAmmo.h"
#include "Player/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Sound/SoundCue.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

AItemAmmo::AItemAmmo()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionObjectType(ECC_Item);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	InfoItemType = EItemType::EIT_Ammo;
}

void AItemAmmo::PlayGainEffect()
{
	if (GainSound)
	{
		UGameplayStatics::PlaySound2D(this, GainSound);
	}
}

void AItemAmmo::HandleItemAfterGain()
{
	Super::HandleItemAfterGain();
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(false);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Widget->SetVisibility(false);
}

void AItemAmmo::BeginPlay()
{
	Super::BeginPlay();

}

void AItemAmmo::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Player = Cast<AAOSCharacter>(OtherActor);
	if (Player)
	{
		Player->SetOverlappingItemCount(1);
		MeshComponent->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Block);
	}
}

void AItemAmmo::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAOSCharacter* Player = Cast<AAOSCharacter>(OtherActor);
	if (Player)
	{
		Player->SetOverlappingItemCount(-1);
		MeshComponent->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Ignore);
	}
}

UStaticMeshComponent* AItemAmmo::GetStaticMesh() const
{
	return MeshComponent;
}

EAmmoType AItemAmmo::GetAmmoType() const
{
	return AmmoType;
}

int32 AItemAmmo::GetAmmoQuantity() const
{
	return AmmoQuantity;
}
