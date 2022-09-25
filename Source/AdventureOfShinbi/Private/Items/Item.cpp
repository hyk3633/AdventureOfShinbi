

#include "Items/Item.h"
#include "Player/AOSCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "HUD/PickupWidget.h"
#include "HUD/InventorySlot.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetVisibility(true);
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetEnableGravity(false);
	ItemMesh->SetCollisionObjectType(ECC_Item);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Block);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(ItemMesh);
	OverlapSphere->SetCollisionObjectType(ECC_ItemRange);
	OverlapSphere->SetGenerateOverlapEvents(true);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	OverlapSphere->SetSphereRadius(100.f);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	Widget->SetupAttachment(ItemMesh);
	Widget->SetVisibility(false);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	SetPickupWidgetInfo();
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Player = Cast<AAOSCharacter>(OtherActor);
	if (Player)
	{
		Player->SetOverlappingItemCount(1);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAOSCharacter* Player = Cast<AAOSCharacter>(OtherActor);
	if (Player)
	{
		Player->SetOverlappingItemCount(-1);
	}
}

void AItem::SetPickupWidgetInfo()
{
	UPickupWidget* PickupWidget = Cast<UPickupWidget>(Widget->GetWidget());

	bool bPickupWidgetValid =
		PickupWidget &&
		PickupWidget->ItemNameText &&
		PickupWidget->ItemTypeText &&
		PickupWidget->ItemRankText;

	if (bPickupWidgetValid)
	{
		PickupWidget->ItemNameText->SetText(FText::FromString(ItemName));
		PickupWidget->ItemTypeText->SetText(FText::FromString(SetItemTypeToWidget(InfoItemType)));
		PickupWidget->ItemRankText->SetText(FText::FromString(SetItemRankToWidget(InfoItemRank)));
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AItem::GetIsWeapon() const
{
	return bIsWeapon;
}

USkeletalMeshComponent* AItem::GetItemMesh() const
{
	return ItemMesh;
}

UWidgetComponent* AItem::GetWidget() const
{
	return Widget;
}

EItemType AItem::GetItemType() const
{
	return InfoItemType;
}

UTexture2D* AItem::GetItemIcon() const
{
	return ItemIcon;
}

void AItem::SetInventorySlot(UInventorySlot* Slot)
{
	InventorySlot = Slot;
}

UInventorySlot* AItem::GetInventorySlot() const
{
	return InventorySlot;
}