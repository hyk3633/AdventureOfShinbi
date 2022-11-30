

#include "Items/ItemRecovery.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

AItemRecovery::AItemRecovery()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionObjectType(ECC_Item);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Block);

	InfoItemType = EItemType::EIT_Recovery;
}

void AItemRecovery::PlayGainEffect()
{
	if (FieldParticleComp)
	{
		FieldParticleComp->Deactivate();
	}
	if (GainParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GainParticle, GetActorLocation(), FRotator::ZeroRotator);
	}
	if (GainSound)
	{
		UGameplayStatics::PlaySound2D(this, GainSound);
	}
}

void AItemRecovery::PlayUsingEffect(FVector Location)
{
	if (UsingParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UsingParticle, Location, FRotator::ZeroRotator);
	}
	if (UsingSound)
	{
		UGameplayStatics::PlaySound2D(this, UsingSound);
	}
}

void AItemRecovery::HandleItemAfterGain()
{
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(false);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Widget->SetVisibility(false);
}

void AItemRecovery::BeginPlay()
{
	Super::BeginPlay();

	if (FieldParticle)
	{
		FieldParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			FieldParticle, 
			ItemMesh, 
			NAME_None, 
			GetActorLocation(), 
			FRotator::ZeroRotator, 
			EAttachLocation::KeepWorldPosition
		);
	}
}

UStaticMeshComponent* AItemRecovery::GetStaticMesh() const
{
	return MeshComponent;
}

ERecoveryType AItemRecovery::GetRecoveryType() const
{
	return RecoveryType;
}

void AItemRecovery::SetQuickSlotIndex(int8 Index)
{
	QuickSlotIndex = Index;
}

int8 AItemRecovery::GetQuickSlotIndex() const
{
	return QuickSlotIndex;
}
