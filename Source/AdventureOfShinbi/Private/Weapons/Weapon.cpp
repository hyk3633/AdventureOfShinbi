

#include "Weapons/Weapon.h"
#include "Player/AOSCharacter.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AWeapon::AWeapon()
{
	bOnSpin = false;
	MovementSpeed = 10.f;
}

void AWeapon::PlayGainEffect()
{
	if (PickAndEquipSound)
	{
		UGameplayStatics::PlaySound2D(this, PickAndEquipSound);
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	ItemMesh->OnComponentHit.AddDynamic(this, &AWeapon::OnHit);
}

void AWeapon::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (DropSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DropSound, GetActorLocation());
	}

	ItemMesh->SetNotifyRigidBodyCollision(false);
}

EWeaponType AWeapon::GetWeaponType() const
{
	return WeaponType;
}

void AWeapon::SetWeaponState(const EWeaponState State)
{
	WeaponState = State;

	switch (State)
	{
	case EWeaponState::EWS_Field:
		ItemMesh->SetVisibility(true);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->BodyInstance.bNotifyRigidBodyCollision = true;
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Block);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

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

		if(GetOwner())
			WeaponOwner = Cast<AAOSCharacter>(GetOwner());

		break;

	case EWeaponState::EWS_Equipped:
		ItemMesh->SetVisibility(true);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->BodyInstance.bNotifyRigidBodyCollision = false;

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		PlayGainEffect();

		if (GetOwner())
			WeaponOwner = Cast<AAOSCharacter>(GetOwner());

		break;
	}

	if (WeaponOwner)
	{
		WeaponOwner->WeaponStateChanged(this);
	}
}

EWeaponState AWeapon::GetWeaponState() const
{
	return WeaponState;
}

void AWeapon::DropWeapon()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	ItemMesh->DetachFromComponent(DetachRules);
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetEnableGravity(true);
	ItemMesh->BodyInstance.bNotifyRigidBodyCollision = true;
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
}
