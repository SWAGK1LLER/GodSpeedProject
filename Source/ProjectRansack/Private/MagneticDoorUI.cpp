#include "MagneticDoorUI.h"

FString UMagneticDoorUI::getTextStateOfficer(bool doorOpen)
{
	return FString("");
}

FString UMagneticDoorUI::getTextStateThief(bool haveCard, bool doorOpen)
{
	if (!haveCard)
		return FString("You don't have the magnetic card to open the door");

	return FString("");
}