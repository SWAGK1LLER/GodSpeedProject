// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorUI.h"

FString UDoorUI::getTextStateOfficer(bool hacked, bool doorOpen)
{
	if (hacked)
		return FString("Hold E to fix the door");

	if (doorOpen)
		return FString("Press E to open door");
	return FString("Press E to close door");
}

FString UDoorUI::getTextStateThief(bool hacked, bool doorOpen)
{
	if (!hacked)
		return FString("Hold E to hack door");

	if (doorOpen)
		return FString("Press E to open door");
	return FString("Press E to close door");
}