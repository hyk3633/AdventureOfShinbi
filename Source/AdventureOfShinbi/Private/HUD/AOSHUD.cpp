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
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * CrosshairSpread;

		if (CrosshairCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(CrosshairCenter, ViewportCenter, Spread);
		}
		if (CrosshairLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(CrosshairLeft, ViewportCenter, Spread);
		}
		if (CrosshairRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(CrosshairRight, ViewportCenter, Spread);
		}
		if (CrosshairTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(CrosshairTop, ViewportCenter, Spread);
		}
		if (CrosshairBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(CrosshairBottom, ViewportCenter, Spread);
		}
	}
}

void AAOSHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
	);
}