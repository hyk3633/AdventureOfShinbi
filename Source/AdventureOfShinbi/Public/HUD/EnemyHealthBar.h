
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.generated.h"

/**
 * �� ü�� ��
 */

class UProgressBar;

UCLASS()
class ADVENTUREOFSHINBI_API UEnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnemyHealthProgressBar;
};
