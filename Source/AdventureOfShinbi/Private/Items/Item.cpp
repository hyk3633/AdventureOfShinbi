

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

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(ItemMesh);
	OverlapSphere->SetCollisionObjectType(ECC_ItemRange);
	OverlapSphere->SetGenerateOverlapEvents(true);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	OverlapSphere->SetSphereRadius(300.f);

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

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpAndDownMovement(DeltaTime);
}

void AItem::UpAndDownMovement(float DeltaTime)
{
	if (bOnUpAndDown)
	{
		FVector NewLocation = GetActorLocation();
		float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
		NewLocation.Z += DeltaHeight * MovementSpeed;
		RunningTime += DeltaTime;
		SetActorLocation(NewLocation);
	}
	if (bOnSpin)
	{
		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += DeltaTime * MovementSpeed;
		SetActorRotation(NewRotation);
	}
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Player = Cast<AAOSCharacter>(OtherActor);
	if (Player)
	{
		// 플레이어의 오버랩된 아이템 카운트를 1 올리고 이 아이템을 트레이스에 블록으로 설정
		Player->SetOverlappingItemCount(1);
		ItemMesh->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Block);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAOSCharacter* Player = Cast<AAOSCharacter>(OtherActor);
	if (Player)
	{
		// 플레이어의 오버랩된 아이템 카운트를 1 내리고 이 아이템을 트레이스에 무시되게 설정
		Player->SetOverlappingItemCount(-1);
		ItemMesh->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Ignore);
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

void AItem::SetItemInventorySlot(UItemInventorySlot* Slot)
{
	ItemInventorySlot = Slot;
}

UInventorySlot* AItem::GetInventorySlot() const
{
	return InventorySlot;
}

UItemInventorySlot* AItem::GetItemInventorySlot() const
{
	return ItemInventorySlot;
}

void AItem::DeactivateItemMovement()
{
	bOnSpin = false;
	bOnUpAndDown = false;
}

void AItem::PlayGainEffect()
{

}

void AItem::HandleItemAfterGain()
{
	DeactivateItemMovement();
}
