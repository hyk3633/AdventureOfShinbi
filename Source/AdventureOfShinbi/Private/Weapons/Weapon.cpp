

#include "Weapons/Weapon.h"
#include "Player/AOSCharacter.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	bIsWeapon = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

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
		ItemMesh->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_EnemyWeaponTrace, ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_PlayerWeaponTrace, ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_ItemRange, ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_EnemyAttackRange, ECollisionResponse::ECR_Ignore);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);

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

		break;
	}
}

EWeaponState AWeapon::GetWeaponState() const
{
	return WeaponState;
}

float AWeapon::GetWeaponDamage()
{
	AAOSCharacter* AC = Cast<AAOSCharacter>(GetOwner());
	if (AC)
	{
		if (AC->GetCombatComp()->GetDmgDebuffActivated())
		{
			return Damage - FMath::RoundToFloat(Damage * 0.3);
		}
		else
		{
			return Damage;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed"));
	return Damage;
}
