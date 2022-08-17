// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AOSController.h"
#include "HUD/AOSCharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "HUD/AOSHUD.h"

void AAOSController::BeginPlay()
{
	Super::BeginPlay();

	AOSHUD = Cast<AAOSHUD>(GetHUD());
}

void AAOSController::SetHUDLoadedAmmoText(int32 Ammo)
{
	AOSHUD = AOSHUD == nullptr ? Cast<AAOSHUD>(GetHUD()) : AOSHUD;

	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->LoadedAmmoText)
	{
		AOSHUD->CharacterOverlay->LoadedAmmoText->SetText(FText::FromString(FString::FromInt(Ammo)));
	}
}

void AAOSController::SetHUDTotalAmmoText(int32 Ammo)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->TotalAmmoText)
	{
		AOSHUD->CharacterOverlay->TotalAmmoText->SetText(FText::FromString(FString::FromInt(Ammo)));
	}
}

void AAOSController::SetHUDAmmoInfoVisibility(bool Visibility)
{
	if (AOSHUD->CharacterOverlay && AOSHUD->CharacterOverlay->AmmoInfoBox)
	{
		ESlateVisibility ESV = Visibility == true ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		AOSHUD->CharacterOverlay->AmmoInfoBox->SetVisibility(ESV);
	}
}
