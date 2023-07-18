// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayerController.h"
#include <Base3C.h>
#include "EOSGameInstance.h"
#include "GameGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/UserWidget.h"
#include <Thief.h>
#include "RoundUI.h"
#include "DuffleBagUI.h"
#include "Item.h"
#include "TeamDuffleBagUI.h"

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(!IsLocalPlayerController())
		return;

	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance == nullptr)
		return;

	SRSpawnPlayer(instance->team);
}

void AGamePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGamePlayerController::PawnIsPossess(APawn* InPawn)
{
	if (!IsLocalPlayerController())
		return;

	AThief* thief = Cast<AThief>(InPawn);
	if (thief == nullptr)
		return;

	RoundUIWidget = CreateWidget<URoundUI>(GetWorld(), RoundUIClass);
	RoundUIWidget->AddToViewport();

	DuffleBagUIWidget = CreateWidget<UDuffleBagUI>(GetWorld(), DuffleBagUIClass);
	DuffleBagUIWidget->AddToViewport();

	TeamDuffleBagUIWidget = CreateWidget<UTeamDuffleBagUI>(GetWorld(), TeamDuffleBagUIClass);
	TeamDuffleBagUIWidget->AddToViewport();

	SRGetTeamData();
	SRUpdatePlayerName();
}

void AGamePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	ABase3C* player = Cast<ABase3C>(InPawn);
	if (player != nullptr)
		player->BindInputHandler();

	PawnIsPossess(InPawn);
}

void AGamePlayerController::SRSpawnPlayer_Implementation(const ETeam& pTeam)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != nullptr)
		gameMode->SpawnPlayer(pTeam, this);
}

void AGamePlayerController::SRUpdatePlayerName_Implementation()
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != nullptr)
		gameMode->SendPlayerName();
}

void AGamePlayerController::SRGetTeamData_Implementation()
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != nullptr)
		gameMode->SendTeamData();
}

void AGamePlayerController::ClientGetTeamA_Implementation(const TArray<ABase3C*>& pTeamA)
{
	TeamA = pTeamA;
	TeamA.Remove(Cast<ABase3C>(GetPawn()));

	UpdateTeamDuffleBagUI();
}

void AGamePlayerController::ClientGetTeamB_Implementation(const TArray<ABase3C*>& pTeamB)
{
	TeamB = pTeamB;
	TeamB.Remove(Cast<ABase3C>(GetPawn()));
}

void AGamePlayerController::UpdateDuffleBagUI(const TArray<FItemLooted>& pItems)
{
	if (!IsLocalPlayerController())
		return;

	DuffleBagUIWidget->UpdateUI(pItems);
}

void AGamePlayerController::UpdateTeamDuffleBagUI()
{
	SRUpdateTeamDuffleBagUI();
}

void AGamePlayerController::SRUpdateTeamDuffleBagUI_Implementation()
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->UpdateTeamDuffleBag();
}

void AGamePlayerController::ClientUpdateTeamDuffleBagUI_Implementation()
{
	if (!IsLocalPlayerController())
		return;

	if (TeamA.Num() > 0)
	{
		AThief* thief = Cast<AThief>(TeamA[0]);

		if (thief->inventory != nullptr)
			TeamDuffleBagUIWidget->UpdateUIBag1(thief->nickName, thief->inventory->items);
		else
			TeamDuffleBagUIWidget->UpdateUIBagName1(thief->nickName);
	}

	if (TeamA.Num() > 1)
	{
		AThief* thief = Cast<AThief>(TeamA[1]);
		if (thief->inventory != nullptr)
			TeamDuffleBagUIWidget->UpdateUIBag2(thief->nickName, thief->inventory->items);
		else
			TeamDuffleBagUIWidget->UpdateUIBagName2(thief->nickName);
	}
}

void AGamePlayerController::ClientUpdateRoundTimeRemaining_Implementation(const FString& pTime)
{
	if (RoundUIWidget == nullptr)
		return;

	RoundUIWidget->SetTime(pTime);
}

void AGamePlayerController::ClientUpdateScore_Implementation(int pTeamA, int pTeamB)
{
	if (RoundUIWidget == nullptr)
		return;

	RoundUIWidget->SetScoreA(pTeamA);
	RoundUIWidget->SetScoreB(pTeamB);
}

UUserWidget* AGamePlayerController::AddInteractibleWidgetUI(AActor* pItem, TSubclassOf<UUserWidget> pWidget)
{
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), pWidget);
	Widget->AddToViewport();

	interactibleUI.Add(pItem, Widget);

	return Widget;
}

void AGamePlayerController::RemoveInteractibleWidgetUI(AActor* pItem)
{
	if (interactibleUI.Find(pItem) == nullptr)
		return;

	interactibleUI[pItem]->RemoveFromParent();
	interactibleUI.Remove(pItem);
}

UUserWidget* AGamePlayerController::GetWidget(AActor* pItem)
{
	if (interactibleUI.Find(pItem) == nullptr)
		return nullptr;

	return interactibleUI[pItem];
}

void AGamePlayerController::SRFreezeInput_Implementation(float duration, ABase3C* actor)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->FreezeInput(duration, actor);
}