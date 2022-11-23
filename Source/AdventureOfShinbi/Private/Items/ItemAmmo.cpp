// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemAmmo.h"
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
	MeshComponent->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Block);
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
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(false);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Widget->SetVisibility(false);
}

void AItemAmmo::BeginPlay()
{
	Super::BeginPlay();

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
