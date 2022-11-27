
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Types/AmmoType.h"
#include "Types/ItemType.h"
#include "AOSPlayerState.generated.h"

/**
 * 
 */

class UTexture2D;
class AWeapon;
class AItem;
class UImage;
class UButton;
class UTextBlock;

UCLASS()
class ADVENTUREOFSHINBI_API AAOSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AAOSPlayerState();

protected:

	virtual void PostInitializeComponents() override;

private:
	
};
