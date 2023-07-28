// Fill out your copyright notice in the Description page of Project Settings.


#include "Thief.h"
#include "Item.h"
#include <GamePlayerController.h>
#include <Kismet/GameplayStatics.h>
#include <GameGameMode.h>

AThief::AThief()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	inventory = NewObject<UInventory>();
}

void AThief::BeginPlay()
{
	Super::BeginPlay();
}

void AThief::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ChangeStencilOnMovement();
}

void AThief::MulReset_Implementation()
{
	Super::MulReset_Implementation();

	AGamePlayerController* pc = Cast<AGamePlayerController>(GetController());
	if (pc == nullptr)
		return;

	pc->ClientFinishArrest();
}

bool AThief::ValidateSpaceItem(AItem& pItem)
{
	return inventory->ValidateSpace(pItem);
}

void AThief::ChangeStencilOnMovement()
{
	if (GetVelocity().Length() > 0 || shouldPingMovement)
	{
		if (GetMesh())
			GetMesh()->SetCustomDepthStencilValue(2); //TODO Implement datatable for thief and add a value for stencil buffer
	}
	else if (shouldPingMovement == false)
	{
		if (GetMesh())
			GetMesh()->SetCustomDepthStencilValue(0);
	}
}

void AThief::SRAddItem_Implementation(AItem* pItem)
{
	pItem->MulPlayerLootIt();
	MUlAddItem(pItem);
}

void AThief::MUlAddItem_Implementation(AItem* pItem)
{
	if (inventory == nullptr)
		inventory = NewObject<UInventory>();

	inventory->AddItem(*pItem);

	AGamePlayerController* PC = Cast<AGamePlayerController>(Controller);
	if (PC != nullptr)
	{
		PC->UpdateDuffleBagUI(inventory->items);
		PC->UpdateTeamDuffleBagUI();
	}
}

void AThief::SRClearItems_Implementation(int score, ETeam pTeam)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	switch (pTeam)
	{
		case ETeam::A : gameMode->AddToScore(score, gameMode->ScoreTeamA);
			break;
		case ETeam::B : gameMode->AddToScore(score, gameMode->ScoreTeamB);
			break;
	}

	MUlClearItems();
}

void AThief::MUlClearItems_Implementation()
{
	inventory->ClearInventory();

	AGamePlayerController* PC = Cast<AGamePlayerController>(Controller);
	if (PC != nullptr)
	{
		PC->UpdateDuffleBagUI(inventory->items);
		PC->UpdateTeamDuffleBagUI();
	}
}

void AThief::SRDropInventory_Implementation(FVector location)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	ETeam team = Cast<UEOSGameInstance>(GetGameInstance())->team;

	switch (team)
	{
	case ETeam::A: gameMode->RemoveToScore(costOnArrest, gameMode->ScoreTeamA);
		break;
	case ETeam::B: gameMode->RemoveToScore(costOnArrest, gameMode->ScoreTeamB);
		break;
	}

	MUlDropInventory(location);
}

void AThief::MUlDropInventory_Implementation(FVector location)
{
	if (inventory == nullptr)
		return;

	for (int i = 0; i < inventory->items.Num(); i++)
	{
		inventory->items[i].item->dropItem(location);
	}

	inventory->ClearInventory();
}

void AThief::Interact()
{
	if (closeItems.Num() == 0)
		return;

	//Should use the camera forward to check wich item to use if multiple in array
	ItemUsing = closeItems[0];
	IThiefInteractibleActor::Execute_Interact(ItemUsing->_getUObject(), this);
}

void AThief::StopInteract()
{
	if (ItemUsing == nullptr)
		return;

	IThiefInteractibleActor::Execute_StopInteract(ItemUsing->_getUObject(), this);
	ItemUsing = nullptr;
}