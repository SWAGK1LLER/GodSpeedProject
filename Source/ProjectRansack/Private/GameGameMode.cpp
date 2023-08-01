// Fill out your copyright notice in the Description page of Project Settings.


#include "GameGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EOSGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "GamePlayerStart.h"
#include "Thief.h"
#include "Officer.h"
#include "GameFramework/PlayerState.h"
#include "GamePlayerController.h"
#include "Particles/ParticleSystemComponent.h"

void AGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance != nullptr)
	{
		//instance->StartGame();
	}

	//For testing
	StartRound();
}

void AGameGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (RoundStarted)
		CurrentTime -= DeltaTime;

	if (CurrentTime <= 0)
		EndRound();

	FString timeRemaining = getRemainingTimeText();
	for (APlayerController* aPC : PC)
	{
		if (!aPC->IsValidLowLevel())
		{
			PC.Remove(aPC);
			continue;
		}

		(Cast<AGamePlayerController>(aPC))->ClientUpdateRoundTimeRemaining(timeRemaining);
	}
}

void AGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PC.Add(NewPlayer);

	if (PC.Num() == MAX_PLAYER)
		StartRound();
}

void AGameGameMode::Logout(AController* ExitPlayer)
{
	APlayerController* playerController = Cast<APlayerController>(ExitPlayer);

	PC.Remove(playerController);

	UEOSGameInstance* instance = Cast<UEOSGameInstance>(GetGameInstance());
	instance->unregisterPlayerToGameSession(playerController);
}

void AGameGameMode::SpawnPlayer(const ETeam& team, APlayerController* NewPlayer)
{
	APawn* pawn = NewPlayer->GetPawn();
	if (pawn != nullptr)
		pawn->K2_DestroyActor();

	FTransform spawnTransform;
	TSubclassOf<AActor> classToSpawn;

	FindSpawn(NewPlayer, team, spawnTransform, classToSpawn);

	ABase3C* actor = Cast<ABase3C>(GetWorld()->SpawnActor(classToSpawn, &spawnTransform));

	switch (team)
	{
	case ETeam::A: TeamA.Add(actor);
		break;
	case ETeam::B: TeamB.Add(actor);
		break;
	default:
		break;
	}

	if (PlayerTeams.Contains(NewPlayer))
		PlayerTeams[NewPlayer] = team;
	else
		PlayerTeams.Add(NewPlayer, team);
	

	NewPlayer->Possess(actor);
}

void AGameGameMode::FindSpawn(APlayerController* NewPlayer, const ETeam& team, FTransform& Location, TSubclassOf<AActor>& ActorClass)
{
	TArray<AActor*> spawnPoint;
	if (team == ETeam::A)
	{
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AGamePlayerStart::StaticClass(), TEAM_A_STR, spawnPoint);
		ActorClass = ThiefClass;
	}
	else if (team == ETeam::B)
	{
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AGamePlayerStart::StaticClass(), TEAM_B_STR, spawnPoint);
		ActorClass = OfficerClass;
	}

	for (AActor* Actor : spawnPoint)
	{
		AGamePlayerStart* spawn = Cast<AGamePlayerStart>(Actor);
		if (spawn->used)
			continue;

		Location = spawn->GetActorTransform();
		spawn->used = true;

		PlayerSpawn.Add(spawn);
		break;
	}
}

void AGameGameMode::SendPlayerName()
{
	for (APlayerController* aPC : PC)
	{
		APawn* pawn = aPC->GetPawn();
		if (pawn == nullptr)
			continue;

		FString name = (aPC->GetPlayerState<APlayerState>())->GetPlayerName();
		(Cast<ABase3C>(pawn))->SetClientNickname(name);
	}
}

void AGameGameMode::SendTeamData()
{
	for (APlayerController* aPC : PC)
	{
		AGamePlayerController* playerController = Cast<AGamePlayerController>(aPC);
		
		playerController->ClientGetTeamA(TeamA);
		playerController->ClientGetTeamB(TeamB);
	}
}

void AGameGameMode::UpdateTeamDuffleBag()
{
	for (ABase3C* player : TeamA)
	{
		AGamePlayerController* aPC = Cast<AGamePlayerController>(player->GetController());
		if(aPC)
			aPC->ClientUpdateTeamDuffleBagUI();
	}
}

void AGameGameMode::SpawnParticle(UParticleSystem* particleEffect, const FTransform& position, const float& duration)
{
	for (APlayerController* aPC : PC)
		(Cast<AGamePlayerController>(aPC))->ClientSpawnParticle(particleEffect, position, duration);
}

FString AGameGameMode::getRemainingTimeText()
{
	return convertTimeToText();
}

FString AGameGameMode::convertTimeToText()
{
	int currTime = (int)CurrentTime;

	int minute = currTime / MINUTE;
	int second = currTime % SECOND;

	return FString::FromInt(minute) + FString(" : ") + (second < 10 ? FString("0") : FString("")) + FString::FromInt(second);
}

void AGameGameMode::AddToScore(int pValue, int& pScore)
{
	pScore += pValue;
	if (pScore < 0)
		pScore = 0;

	for (APlayerController* aPC : PC)
		(Cast<AGamePlayerController>(aPC))->ClientUpdateScore(ScoreTeamA, ScoreTeamB);
}

void AGameGameMode::RemoveToScore(int pValue, int& pScore)
{
	pScore -= pValue;
	if (pScore < 0)
		pScore = 0;

	for (APlayerController* aPC : PC)
		(Cast<AGamePlayerController>(aPC))->ClientUpdateScore(ScoreTeamA, ScoreTeamB);
}

void AGameGameMode::FreezeInput(float duration, ABase3C* actor)
{
	actor->ClientFreezeInput(duration);
}

void AGameGameMode::ArrestThief(ABase3C* other)
{
	AGamePlayerController* playerController = Cast<AGamePlayerController>(other->GetController());
	if (playerController == nullptr)
		return;

	playerController->ClientBeingArrest();
}

void AGameGameMode::StartRound()
{
	//TO DO: add cooldown before starting round

	CurrentTime = Timer * MINUTE;
	RoundStarted = true;
	TotalRound++;
}

void AGameGameMode::EndRound()
{
	RoundStarted = false;

	if (TotalRound == MAX_ROUND)
	{
		EndGame();
		return;
	}

	//TO DO: Add cooldown before restarting round
	for (int i = 0; i < PlayerSpawn.Num(); i++)
		PlayerSpawn[i]->used = false;

	TeamA.Empty();
	TeamB.Empty();

	std::swap(ScoreTeamA, ScoreTeamB);

	for (int i = 0; i < PC.Num(); i++)
	{
		Cast<AGamePlayerController>(PC[i])->RestartRound();
		(Cast<AGamePlayerController>(PC[i]))->ClientUpdateScore(ScoreTeamA, ScoreTeamB);
	}

	//TO DO call back when finish restarting
	StartRound();
}

void AGameGameMode::EndGame()
{
	//TO DO: Show score tab

	bool teamAWin = ScoreTeamA > ScoreTeamB;
	bool teamBWin = !teamAWin;
	bool tie = ScoreTeamA == ScoreTeamB;

	for (int i = 0; i < PC.Num(); i++)
	{
		if (PlayerTeams[PC[i]] == ETeam::A)
			Cast<AGamePlayerController>(PC[i])->EndGame(teamAWin, tie);
		else
			Cast<AGamePlayerController>(PC[i])->EndGame(teamBWin, tie);
	}

	PC.Empty();
}