

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AOSCharacterOverlay.generated.h"

/**
 * 
 */

class UProgressBar;
class UTextBlock;
class UHorizontalBox;

UCLASS()
class ADVENTUREOFSHINBI_API UAOSCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AmmoInfoBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadedAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalAmmoText;
	
};
