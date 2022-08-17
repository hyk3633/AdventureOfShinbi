// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AOSController.generated.h"

/**
 * 
 */

class AAOSHUD;

UCLASS()
class ADVENTUREOFSHINBI_API AAOSController : public APlayerController
{
	GENERATED_BODY()
	
public:

	void SetHUDLoadedAmmoText(int32 Ammo);
	void SetHUDTotalAmmoText(int32 Ammo);
	void SetHUDAmmoInfoVisibility(bool Visibility);

protected:

	virtual void BeginPlay() override;

private:

	AAOSHUD* AOSHUD;

};
