// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemAmmo.h"

AItemAmmo::AItemAmmo()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);


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
