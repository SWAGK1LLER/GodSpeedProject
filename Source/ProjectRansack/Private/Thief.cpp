// Fill out your copyright notice in the Description page of Project Settings.


#include "Thief.h"
#include <GamePlayerController.h>
#include <Kismet/GameplayStatics.h>

AThief::AThief()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	inventory = NewObject<UInventory>();
}

bool AThief::ValidateSpaceItem(AItem& pItem)
{
	return inventory->ValidateSpace(pItem);
}

void AThief::AddItem(AItem& pItem)
{
	if (inventory == nullptr)
		inventory = NewObject<UInventory>();

	inventory->AddItem(pItem);


	AGamePlayerController* PC = Cast<AGamePlayerController>(Controller);
	if (PC == nullptr || !PC->IsLocalPlayerController())
		return;

	PC->UpdateDuffleBagUI(inventory->items);
	PC->UpdateTeamDuffleBagUI();
	
}

void AThief::Interact()
{
	if (closeItems.Num() == 0)
		return;

	//Should use the camera forward to check wich item to use if multiple in array
	ItemUsing = closeItems[0];
	IThiefInteractibleActor::Execute_Interact((UObject*)(ItemUsing));
}

void AThief::StopInteract()
{
	if (ItemUsing == nullptr)
		return;

	IThiefInteractibleActor::Execute_StopInteract((UObject*)(ItemUsing));
	ItemUsing = nullptr;
}