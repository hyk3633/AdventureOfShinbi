// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AOSHUD.generated.h"

/**
 * 
 */

class UUserWidget;
class UAOSCharacterOverlay;

UCLASS()
class ADVENTUREOFSHINBI_API AAOSHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UAOSCharacterOverlay* CharacterOverlay;

protected:

	virtual void BeginPlay() override;

	void AddOverlay();

private:


};
