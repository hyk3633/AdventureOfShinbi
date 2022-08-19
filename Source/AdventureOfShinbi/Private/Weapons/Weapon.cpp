

#include "Weapons/Weapon.h"
#include "Components/SphereComponent.h"
#include "Player/AOSCharacter.h"
#include "Components/WidgetComponent.h"
#include "HUD/PickupWidget.h"
#include "Components/TextBlock.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(WeaponMesh);
	OverlapSphere->SetSphereRadius(100.f);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	Widget->SetupAttachment(WeaponMesh);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);

	SetPickupWidgetInfo();
	Widget->SetVisibility(false);
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
