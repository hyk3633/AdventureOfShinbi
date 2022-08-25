

#include "Weapons/Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Player/AOSCharacter.h"
#include "HUD/PickupWidget.h"
#include "HUD/InventorySlot.h"
#include "Kismet/GameplayStatics.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(WeaponMesh);
	OverlapSphere->SetSphereRadius(100.f);

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	DamageCollision->SetupAttachment(RootComponent);

	DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	DamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	Widget->SetupAttachment(WeaponMesh);
	Widget->SetVisibility(false);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnDamageCollisionOverlap);

	SetPickupWidgetInfo();
	SetWeaponState(EWeaponState::EWS_Field);


}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Player = Cast<AAOSCharacter>(OtherActor);
	if (Player)
	{
		Player->SetOverlappingWeapon(this);
	}
	if (Widget)
	{
		Widget->SetVisibility(true);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAOSCharacter* Player = Cast<AAOSCharacter>(OtherActor);
	if (Player)
	{
		Player->SetOverlappingWeapon(nullptr);
	}
	if (Widget)
	{
		Widget->SetVisibility(false);
	}
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

void AWeapon::SetPickupWidgetInfo()
{
	UPickupWidget* PickupWidget = Cast<UPickupWidget>(Widget->GetWidget());

	bool bPickupWidgetValid =
		PickupWidget &&
		PickupWidget->ItemNameText &&
		PickupWidget->ItemTypeText &&
		PickupWidget->ItemRankText;

	if (bPickupWidgetValid)
	{
		PickupWidget->ItemNameText->SetText(FText::FromString(WeaponName));
		PickupWidget->ItemTypeText->SetText(FText::FromString(SetItemTypeToWidget(InfoItemType)));
		PickupWidget->ItemRankText->SetText(FText::FromString(SetItemRankToWidget(InfoItemRank)));
	}
}

EWeaponType AWeapon::GetWeaponType() const
{
	return WeaponType;
}

USkeletalMeshComponent* AWeapon::GetWeaponMesh() const
{
	return WeaponMesh;
}

UWidgetComponent* AWeapon::GetWidget() const
{
	return Widget;
}

UTexture2D* AWeapon::GetWeaponIcon() const
{
	return WeaponIcon;
}

void AWeapon::SetWeaponState(const EWeaponState State)
{
	WeaponState = State;

	WeaponStateChanged.Broadcast(this);

	switch (State)
	{
	case EWeaponState::EWS_Field:
		WeaponMesh->SetVisibility(true);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		Widget->SetVisibility(false);
		break;

	case EWeaponState::EWS_PickedUp:
	case EWeaponState::EWS_QuickSlot1:
	case EWeaponState::EWS_QuickSlot2:
		WeaponMesh->SetVisibility(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		
		break;

	case EWeaponState::EWS_Equipped:
		WeaponMesh->SetVisibility(true);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		DamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		
		break;

	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetVisibility(true);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		
		break;
	}
}

EWeaponState AWeapon::GetWeaponState() const
{
	return WeaponState;
}

void AWeapon::SetInventorySlot(UInventorySlot* Slot)
{
	InventorySlot = Slot;
}

UInventorySlot* AWeapon::GetInventorySlot() const
{
	return InventorySlot;
}
