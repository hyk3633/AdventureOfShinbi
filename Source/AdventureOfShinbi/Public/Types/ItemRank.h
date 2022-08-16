#pragma once

UENUM(BlueprintType)
enum class EItemRank : uint8
{   // �ʷ� �Ķ� ���� ��ȫ ��Ȳ
	EIR_Common UMETA(DisplayName = "Common"), 
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Unique UMETA(DisplayName = "Unique"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};