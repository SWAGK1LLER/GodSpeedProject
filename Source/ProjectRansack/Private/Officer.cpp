// Fill out your copyright notice in the Description page of Project Settings.


#include "Officer.h"

void AOfficer::Interact()
{
	if (closeItems.Num() == 0)
		return;

	//Should use the camera forward to check wich item to use if multiple in array
	ItemUsing = closeItems[0];
	IOfficerInteractibleActor::Execute_Interact((UObject*)(ItemUsing));
}

void AOfficer::StopInteract()
{
	if (ItemUsing == nullptr)
		return;

	IOfficerInteractibleActor::Execute_StopInteract((UObject*)(ItemUsing));
	ItemUsing = nullptr;
}