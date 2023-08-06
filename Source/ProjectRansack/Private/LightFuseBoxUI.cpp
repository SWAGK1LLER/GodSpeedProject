// Fill out your copyright notice in the Description page of Project Settings.


#include "LightFuseBoxUI.h"

FString ULightFuseBoxUI::getTextStateOfficer(bool lightOpen)
{
	if (lightOpen)
		return FString("Press E to turn off all light");
	return FString("Press E to turn on all light");
}

FString ULightFuseBoxUI::getTextStateThief(bool hacked, bool lightOpen)
{
	if (!hacked)
		return FString("Hold E to hack fuse box");

	if (lightOpen)
		return FString("Press E to turn off all light");
	return FString("Press E to turn on all light");
}