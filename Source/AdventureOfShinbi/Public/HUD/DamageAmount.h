// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageAmount.generated.h"

/**
 * 
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
