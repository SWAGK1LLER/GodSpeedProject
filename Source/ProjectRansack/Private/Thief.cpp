// Fill out your copyright notice in the Description page of Project Settings.


#include "Thief.h"
#include <GamePlayerController.h>
#include <Kismet/GameplayStatics.h>

bool AThief::ValidateSpaceItem(AItem& pItem)
{
	return inventory.ValidateSpace(pItem);
}

void AThief::AddItem(AItem& pItem)
{
	inventory.AddItem(pItem);

	AGamePlayerController* PC = Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC == nullptr)
		return;

	PC->UpdateInventoryUI(inventory.items);
}

