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
#include "Particles/ParticleSystemComponent.h"
#include "GenericParticleSystemComponent.h"
#include <Officer.h>
#include "LightFuseBoxe.h"
#include <Door.h>
#include "Terminal.h"

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(!IsLocalPlayerController())
		return;

	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance == nullptr)
		return;

	if (RoundUIWidget == nullptr)
	{
		RoundUIWidget = CreateWidget<URoundUI>(GetWorld(), RoundUIClass);
		RoundUIWidget->AddToViewport();
	}

	instance->LoadSaveGame();
	//instance->LoadSaveSettings();

	
	SRSpawnPlayer(instance->team);
}

void AGamePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RespawnUI != nullptr)
	{
		respawnCurrentTime -= DeltaTime;
		RespawnUI->SetTime(respawnCurrentTime);

		if (respawnCurrentTime <= 0)
		{
			RespawnUI->RemoveFromParent();
			RespawnUI = nullptr;
		}
	}
}

void AGamePlayerController::PawnIsPossess(APawn* InPawn)
{
	if (!IsLocalPlayerController())
		return;

	AThief* thief = Cast<AThief>(InPawn);
	if (thief == nullptr)
	{
		AOfficer* officer = Cast<AOfficer>(InPawn);
		if (officer != nullptr)
			officer->SetClientUI();
		return;
	}
		

	SetUpUI(thief);
	SRGetTeamData();
	SRUpdatePlayerName();
}

void AGamePlayerController::SetUpUI_Implementation(APawn* InPawn)
{
	AThief* thief = Cast<AThief>(InPawn);
	if (thief == nullptr)
		return;

	if (DuffleBagUIWidget == nullptr)
	{
		DuffleBagUIWidget = CreateWidget<UDuffleBagUI>(GetWorld(), DuffleBagUIClass);
		DuffleBagUIWidget->AddToViewport();
	}
	else
	{
		if (thief->inventory == nullptr)
			thief->inventory = NewObject<UInventory>();

		DuffleBagUIWidget->UpdateUI(thief->inventory->items);
	}

	if (TeamDuffleBagUIWidget == nullptr)
	{
		TeamDuffleBagUIWidget = CreateWidget<UTeamDuffleBagUI>(GetWorld(), TeamDuffleBagUIClass);
		TeamDuffleBagUIWidget->AddToViewport();
	}

	thief->SetClientUI();
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
		if (thief == nullptr)
			return;

		if (thief->inventory != nullptr)
			TeamDuffleBagUIWidget->UpdateUIBag1(thief->nickName, thief->inventory->items);
		else
			TeamDuffleBagUIWidget->UpdateUIBagName1(thief->nickName);
	}

	if (TeamA.Num() > 1)
	{
		AThief* thief = Cast<AThief>(TeamA[1]);
		if (thief == nullptr)
			return;

		if (thief->inventory != nullptr)
			TeamDuffleBagUIWidget->UpdateUIBag2(thief->nickName, thief->inventory->items);
		else
			TeamDuffleBagUIWidget->UpdateUIBagName2(thief->nickName);
	}
}

void AGamePlayerController::ClientUpdateRoundTimeRemaining_Implementation(const FString& pTime)
{
	if (RoundUIWidget == nullptr || !IsLocalPlayerController())
		return;

	RoundUIWidget->SetTime(pTime);
}

void AGamePlayerController::ClientUpdateCustomTimer_Implementation(const FString& text, const float& pTime)
{
	if (RoundUIWidget == nullptr || !IsLocalPlayerController())
		return;

	RoundUIWidget->SetTextTimer(text, pTime);
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

void AGamePlayerController::SRHackLights_Implementation(ALightFuseBoxe* actor)
{
	actor->HackLights();
}

void AGamePlayerController::SRToggleLights_Implementation(ALightFuseBoxe* actor, bool pOpen)
{
	actor->ToggleLights(pOpen);
}

void AGamePlayerController::SRHackDoor_Implementation(ADoor* actor)
{
	actor->HackDoor();
}

void AGamePlayerController::SRToggleCameras_Implementation(ATerminal* actor, bool pOn) //TODO 
{
	actor->DisableCameras(pOn);
}

void AGamePlayerController::SRHackTerminal_Implementation(ATerminal* actor)
{
	actor->HackTerminal();
	actor->UpdateUIText();
}

void AGamePlayerController::SRFixTerminal_Implementation(ATerminal* actor)
{
	actor->FixTerminal();
	actor->UpdateUIText();
}

void AGamePlayerController::SRToggleDoor_Implementation(ADoor* actor, bool pOpen)
{
	actor->ToggleDoor(pOpen);
}

void AGamePlayerController::SRFreezeInput_Implementation(float duration, ABase3C* actor)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->FreezeInput(duration, actor);
}

void AGamePlayerController::CameraFreezeInput_Implementation(AActor* actor)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->FreezeCamera(actor);
}

void AGamePlayerController::SRSpawnParticle_Implementation(UParticleSystem* particleEffect, const FTransform& position, const float& duration)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->SpawnParticle(particleEffect, position, duration);
}

void AGamePlayerController::ClientSpawnParticle_Implementation(UParticleSystem* particleEffect, const FTransform& position, const float& duration)
{
	UGenericParticleSystemComponent* particle = (UGenericParticleSystemComponent*)UGameplayStatics::SpawnEmitterAtLocation(
																															GetWorld(),
																															particleEffect,
																															position,
																															true,
																															EPSCPoolMethod::AutoRelease,
																															true
																														);
	if (duration != -1)
		particle->setLifeSpan(duration);
}

void AGamePlayerController::ClientBeingArrest_Implementation()
{
	AThief* thief = Cast<AThief>(GetPawn());
	if (thief == nullptr)
		return;

	thief->SRDropInventory(thief->GetActorLocation());
	thief->SRReset();
}

void AGamePlayerController::ClientFinishArrest_Implementation(float respawnDuration)
{
	AThief* thief = Cast<AThief>(GetPawn());
	if (thief == nullptr)
		return;

	for (auto& it : interactibleUI)
	{
		it.Value->RemoveFromParent();
	}

	UpdateDuffleBagUI(thief->inventory->items);
	UpdateTeamDuffleBagUI();

	if (RespawnUI == nullptr)
	{
		respawnCurrentTime = respawnDuration;
		RespawnUI = CreateWidget<URespawnUI>(GetWorld(), RespawnWidgetClass);
		RespawnUI->AddToViewport();
	}
}

void AGamePlayerController::RestartRound_Implementation()
{
	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance == nullptr)
		return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItem::StaticClass(), FoundActors);

	for (int i = 0; i < FoundActors.Num(); i++)
		Cast<AItem>(FoundActors[i])->reset();

	instance->team = instance->team == ETeam::A ? ETeam::B : ETeam::A;

	ABase3C* base3c = Cast<ABase3C>(GetPawn());
	if (base3c->WidgetUI != nullptr)
	{
		base3c->WidgetUI->RemoveFromParent();
		base3c->WidgetUI = nullptr;
	}
	

	if (DuffleBagUIWidget != nullptr)
	{
		DuffleBagUIWidget->RemoveFromParent();
		DuffleBagUIWidget = nullptr;
	}

	if (TeamDuffleBagUIWidget != nullptr)
	{
		TeamDuffleBagUIWidget->RemoveFromParent();
		TeamDuffleBagUIWidget = nullptr;
	}

	int playTimeDiff = UGameplayStatics::GetRealTimeSeconds((GetWorld())) - base3c->playTime;
	if (base3c->IsA(AThief::StaticClass()))
		instance->GetPlayerSaveGame()->timePlayThief += playTimeDiff;
	else
		instance->GetPlayerSaveGame()->timePlaySecurity += playTimeDiff;


	SRSpawnPlayer(instance->team);
}

void AGamePlayerController::EndGame_Implementation(bool isWin, bool isTie)
{
	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance == nullptr)
		return;

	if (instance->GetPlayerSaveGame() == nullptr)
		return;

	if (isTie)
	{
		instance->SaveGameFinish.AddDynamic(this, &AGamePlayerController::SaveGameFinish);
		instance->SaveGame();
		return;
	}

	if (isWin)
		instance->GetPlayerSaveGame()->totalWin++;
	else
		instance->GetPlayerSaveGame()->totalLoose++;

	instance->SaveGameFinish.AddDynamic(this, &AGamePlayerController::SaveGameFinish);
	instance->SaveGame();
}

void AGamePlayerController::SaveGameFinish()
{
	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance == nullptr)
		return;

	instance->SaveGameFinish.RemoveDynamic(this, &AGamePlayerController::SaveGameFinish);
	UGameplayStatics::OpenLevel(GetGameInstance(), FName("Game/Maps/MainMenu"));
}

void AGamePlayerController::SRBeginArrestThief_Implementation(AThief* pThief, bool pArrest, AOfficer* pOfficer)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->beginArrestThief(pArrest, pThief, pOfficer);
}