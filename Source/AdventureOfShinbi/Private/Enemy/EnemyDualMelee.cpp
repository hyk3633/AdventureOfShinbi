// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyDualMelee.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Components/BoxComponent.h"

AEnemyDualMelee::AEnemyDualMelee()
{

	DamageCollision2 = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageCollision 2"));
	DamageCollision2->SetupAttachment(GetMesh(), FName("EnemySocket2"));
	DamageCollision2->SetCollisionObjectType(ECC_EnemyWeapon);
	DamageCollision2->SetGenerateOverlapEvents(false);
	DamageCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageCollision2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DamageCollision2->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
}

void AEnemyDualMelee::BeginPlay()
{
	Super::BeginPlay();

	DamageCollision2->OnComponentBeginOverlap.AddDynamic(this, &AEnemyDualMelee::OnDamageCollisionOverlap);
}

void AEnemyDualMelee::Tick(float DeltaTime)
{

}

void AEnemyDualMelee::OnDamageCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnDamageCollisionOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AEnemyDualMelee::ActivateDamageCollision2()
{
	DamageCollision2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyDualMelee::DeactivateDamageCollision2()
{
	DamageCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
