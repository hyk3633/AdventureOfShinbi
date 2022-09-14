

#include "Weapons/Weapon.h"
#include "Player/AOSCharacter.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh->SetVisibility(true);
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetEnableGravity(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	DamageCollision->SetupAttachment(RootComponent);
	DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	bIsWeapon = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnDamageCollisionOverlap);

}

void AWeapon::OnDamageCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Causer = Cast<AAOSCharacter>(GetOwner());
	if (Causer)
	{
		AController* CauserController = Cast<AController>(Causer->Controller);
		if (CauserController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, MeleeDamage, CauserController, Causer, UDamageType::StaticClass());
		}
	}
}

UBoxComponent* AWeapon::GetDamageCollision() const
{
	return DamageCollision;
}

EWeaponType AWeapon::GetWeaponType() const
{
	return WeaponType;
}

void AWeapon::SetWeaponState(const EWeaponState State)
{
	WeaponState = State;

	WeaponStateChanged.Broadcast(this);

	switch (State)
	{
	case EWeaponState::EWS_Field:
		ItemMesh->SetVisibility(true);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		Widget->SetVisibility(false);
		break;

	case EWeaponState::EWS_PickedUp:
	case EWeaponState::EWS_QuickSlot1:
	case EWeaponState::EWS_QuickSlot2:
		ItemMesh->SetVisibility(false);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		
		Widget->SetVisibility(false);
		break;

	case EWeaponState::EWS_Equipped:
		ItemMesh->SetVisibility(true);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DamageCollision->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Overlap);
		
		break;
	}
}

EWeaponState AWeapon::GetWeaponState() const
{
	return WeaponState;
}
