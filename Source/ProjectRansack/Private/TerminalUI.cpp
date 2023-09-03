// Fill out your copyright notice in the Description page of Project Settings.


#include "TerminalUI.h"

FString UTerminalUI::getTextStateOfficer(bool hacked, bool CamerasOff)
{
	if (hacked)
		return FString("Hold E to Fix Terminal");

	if (CamerasOff)
		return FString("Press E to disable Cameras");
	return FString("Press E to enable Cameras");
}

FString UTerminalUI::getTextStateThief(bool hacked, bool CamerasOff)
{
	if (!hacked)
		return FString("Hold E to hack Terminal");

	if (CamerasOff)
		return FString("Press E to disable Cameras");
	return FString("Press E to enable Cameras");
}
