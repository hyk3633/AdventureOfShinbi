
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageAmount.generated.h"

/**
 * ������ ��ġ ����
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
