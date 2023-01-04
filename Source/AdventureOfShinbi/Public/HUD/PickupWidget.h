
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickupWidget.generated.h"

/**
 * ������ ���� ���� Ŭ����
 */

class UTextBlock;

UCLASS()
class ADVENTUREOFSHINBI_API UPickupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemTypeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemRankText;
};
