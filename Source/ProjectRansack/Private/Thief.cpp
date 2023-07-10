// Fill out your copyright notice in the Description page of Project Settings.


#include "Thief.h"
#include <GamePlayerController.h>
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>

AThief::AThief()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	inventory = NewObject<UInventory>();
}

void AThief::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
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

	PC->UpdateInventoryUI(inventory->items);
	PC->UpdateInventoryTeamUI();
	
}