
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageAmount.generated.h"

/**
 * 데미지 수치 위젯
 */

class UTextBlock;

UCLASS()
class ADVENTUREOFSHINBI_API UDamageAmount : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Popup;

};
