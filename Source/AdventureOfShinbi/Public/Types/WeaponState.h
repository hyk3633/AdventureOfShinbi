#pragma once

UENUM()
enum class EWeaponState : uint8
{
	EWS_Field,
	EWS_PickedUp,
	EWS_QuickSlot1,
	EWS_QuickSlot2,
	EWS_Equipped,
	EWS_Dropped,

	EWS_MAX
};