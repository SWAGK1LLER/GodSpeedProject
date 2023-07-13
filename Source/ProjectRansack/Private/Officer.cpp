// Fill out your copyright notice in the Description page of Project Settings.


#include "Officer.h"

void AOfficer::Interact()
{
	if (closeItems.Num() == 0)
		return;

	IOfficerInteractibleActor::Execute_Interact((UObject*)(closeItems[0]));
}