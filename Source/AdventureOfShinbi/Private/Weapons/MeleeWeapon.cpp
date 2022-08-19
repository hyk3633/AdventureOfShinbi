// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/MeleeWeapon.h"
#include "Components/BoxComponent.h"
#include "Player/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"

AMeleeWeapon::AMeleeWeapon()
{

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	DamageCollision->SetupAttachment(RootComponent);
	DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	DamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnDamageCollisionOverlap);
}

void AMeleeWeapon::OnDamageCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Causer = Cast<AAOSCharacter>(GetOwner());
	if (Causer)
	{
		AController* CauserController = Cast<AController>(Causer->Controller);
		if (CauserController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, CauserController, Causer, UDamageType::StaticClass());
		}
	}
}
