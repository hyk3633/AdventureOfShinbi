// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AOSHUD.h"
#include "GameFramework/PlayerController.h"
#include "HUD/AOSCharacterOverlay.h"
#include "Components/TextBlock.h"

void AAOSHUD::BeginPlay()
{
	Super::BeginPlay();

	AddOverlay();
}

void AAOSHUD::AddOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UAOSCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AAOSHUD::DrawHUD()
{

}