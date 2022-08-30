

#include "Items/ItemRecovery.h"

AItemRecovery::AItemRecovery()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	InfoItemType = EItemType::EIT_Recovery;
}

void AItemRecovery::BeginPlay()
{
	Super::BeginPlay();


}

UStaticMeshComponent* AItemRecovery::GetStaticMesh() const
{
	return MeshComponent;
}

ERecoveryType AItemRecovery::GetRecoveryType() const
{
	return RecoveryType;
}
