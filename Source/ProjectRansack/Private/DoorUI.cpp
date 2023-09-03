// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorUI.h"

FString UDoorUI::getTextStateOfficer(bool lightOpen)
{
	if (lightOpen)
		return FString("Press E to open door");
	return FString("Press E to close door");
}

FString UDoorUI::getTextStateThief(bool hacked, bool lightOpen)
{
	if (!hacked)
		return FString("Hold E to hack door");

	if (lightOpen)
		return FString("Press E to open door");
	return FString("Press E to close door");
}