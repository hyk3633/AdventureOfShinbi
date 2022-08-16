

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AOSCharacterOverlay.generated.h"

/**
 * 
 */

class UProgressBar;
class UTextBlock;

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
	UTextBlock* RoadedAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalAmmoText;
	
};
