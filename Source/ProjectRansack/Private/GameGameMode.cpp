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
#include "SecurityCamera.h"
#include <ExtractionZone.h>

#define FORCE_SPAWN_BOTH_SIDE 0

void AGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance != nullptr)
	{
		instance->StartGame();
	}

	StartWarmup();
}

void AGameGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (eventTimer.used)
	{
		eventTimer.currentTime -= DeltaTime;
		eventTimer.onTick.Execute();

		if (eventTimer.currentTime < 0)
		{
			eventTimer.used = false;
			eventTimer.onFinish.Execute();
		}
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

void AGameGameMode::SpawnPlayer(ETeam team, APlayerController* NewPlayer)
{
	if (FORCE_SPAWN_BOTH_SIDE)
	{
		team = ETeam::A;
		static int a = 0;
		if (a <= 1)
		{
			team = ETeam::B;
			a++;
		}
	}

	APawn* pawn = NewPlayer->GetPawn();
	if (pawn != nullptr)
	{
		if (TeamA.Contains(pawn))
			TeamA.Remove(Cast<ABase3C>(pawn));

		pawn->K2_DestroyActor();
	}

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
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AGamePlayerStart::StaticClass(), *extractSpawnTag, spawnPoint);
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
	int currTime = (int)eventTimer.currentTime;

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

void AGameGameMode::FreezeCamera(AActor* actor)
{
	Cast<ASecurityCamera>(actor)->Mul_FreezeCamera();
}

void AGameGameMode::ArrestThief(ABase3C* other)
{
	AGamePlayerController* playerController = Cast<AGamePlayerController>(other->GetController());
	if (playerController == nullptr)
		return;

	playerController->ClientBeingArrest();
}

void AGameGameMode::StartWarmup()
{
	FOnCustomTimerTick onTick;
	onTick.BindLambda([this]
		{
			for (APlayerController* aPC : PC)
			{
				if (!aPC->IsValidLowLevel())
				{
					PC.Remove(aPC);
					continue;
				}

				(Cast<AGamePlayerController>(aPC))->ClientUpdateCustomTimer("Round Start In ", eventTimer.currentTime);
			}
		});
	FOnCustomTimerFinish onFinish;
	onFinish.BindUObject(this, &AGameGameMode::StartRound);
	SetCustomTimerCallback(warmupTimer, onTick, onFinish);

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AExtractionZone::StaticClass(), actors);

	if (actors.Num() != 0)
		extractSpawnTag.AppendInt(FMath::RandRange(1, actors.Num() > 3 ? 3 : actors.Num()));
}

void AGameGameMode::StartRound()
{
	FOnCustomTimerTick onTick;
	onTick.BindLambda([this]
		{
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
		});
	FOnCustomTimerFinish onFinish;
	onFinish.BindUObject(this, &AGameGameMode::EndRound);
	SetCustomTimerCallback(RoundTimer, onTick, onFinish);

	for (APlayerController* aPC : PC)
	{
		((ABase3C*)aPC->GetPawn())->UnFreezeInput();
	}

	TotalRound++;
}

void AGameGameMode::EndRound()
{
	if (TotalRound == MAX_ROUND)
	{
		EndGame();
		return;
	}

	FOnCustomTimerTick onTick;
	onTick.BindLambda([this]
		{
			FString timeRemaining = getRemainingTimeText();
			for (APlayerController* aPC : PC)
			{
				if (!aPC->IsValidLowLevel())
				{
					PC.Remove(aPC);
					continue;
				}

				(Cast<AGamePlayerController>(aPC))->ClientUpdateCustomTimer("Round End in ", eventTimer.currentTime);
			}
		});
	FOnCustomTimerFinish onFinish;
	onFinish.BindUObject(this, &AGameGameMode::RestartRound);
	SetCustomTimerCallback(EndTimer, onTick, onFinish);
}

void AGameGameMode::RestartRound()
{
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

	StartWarmup();
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

void AGameGameMode::SetCustomTimerCallback(float time, const FOnCustomTimerTick& onTick, const FOnCustomTimerFinish& onFinish)
{
	eventTimer.used = true;
	eventTimer.currentTime = time;

	eventTimer.onTick.Unbind();
	eventTimer.onFinish.Unbind();

	eventTimer.onTick = onTick;
	eventTimer.onFinish = onFinish;
}

void AGameGameMode::beginArrestThief(bool pArrest, AThief* pThief, AOfficer* pOfficer)
{
	pThief->MulSetBeingArrest(pArrest, pOfficer);
}