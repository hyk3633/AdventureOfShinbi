#pragma once

// Åº¾à À¯Çü : ar, smg, shell, pistol, grenade, gl, rocket

UENUM()
enum class EAmmoType : uint8
{
	EAT_AR,
	EAT_Sniper,
	EAT_Pistol,
	EAT_Shell,
	EAT_GrenadeLauncher,
	EAT_Rocket,

	EAT_MAX
};