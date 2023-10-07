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

int AGameGameMode::teamCount = 0;

void AGameGameMode::BeginPlay()
{
	AGameGameMode::teamCount = 0;

	Super::BeginPlay();

	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance != nullptr)
	{
		instance->StartGame();
		TotalPlayerToStart = instance->TotalConnectedPlayer;
	}

	StartWaitingConnection();
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
	if (debugForceSpawnBothSide)
	{
		team = ETeam::A;
		if (teamCount < 1)
		{
			team = ETeam::B;
			teamCount++;
		}
	}
	else if (debugEnabled)
	{
		team = debugSpawnedTeam;
	}


	APawn* pawn = NewPlayer->GetPawn();
	if (pawn != nullptr)
	{
		if (TeamA.Contains(pawn))
			TeamA.Remove(Cast<ABase3C>(pawn));

		pawn->K2_DestroyActor();
	}

	FTransform spawnTransform;
	TSubclassOf<ABase3C> classToSpawn;

	FindSpawn(NewPlayer, team, spawnTransform, classToSpawn);

	ABase3C* actor = GetWorld()->SpawnActor<ABase3C>(classToSpawn, spawnTransform, FActorSpawnParameters());
	if (actor == nullptr)
		return;

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
	spawnedPlayer++;

	if (roundStarted)
	{
		actor->UnFreezeInput();
	}
}

void AGameGameMode::FindSpawn(APlayerController* NewPlayer, const ETeam& team, FTransform& Location, TSubclassOf<ABase3C>& ActorClass)
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

void AGameGameMode::FreezeInput(float duration, ABase3C* actor, FVector DamageActorLocation)
{
	actor->ClientFreezeInput(duration, DamageActorLocation);
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

void AGameGameMode::StartWaitingConnection()
{
	roundStarted = false;

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

			(Cast<AGamePlayerController>(aPC))->ClientUpdateCustomTimer("Waiting for other player ", eventTimer.currentTime);
		}

		if (spawnedPlayer == MAX_PLAYER || TotalPlayerToStart == spawnedPlayer)
		{
			eventTimer.currentTime = 0;
			eventTimer.used = false;
			eventTimer.onFinish.Execute();
		}
	});

	FOnCustomTimerFinish onFinish;
	onFinish.BindUObject(this, &AGameGameMode::StartWarmup);
	SetCustomTimerCallback(connectionWaiter, onTick, onFinish);
}

void AGameGameMode::StartWarmup()
{
	roundStarted = false;

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
	roundStarted = true;

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
		if (aPC == nullptr)
			continue;

		if (aPC->GetPawn() == nullptr)
			continue;

		((ABase3C*)aPC->GetPawn())->UnFreezeInput();
	}

	TotalRound++;
}

void AGameGameMode::EndRound()
{
	roundStarted = false;

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

void AGameGameMode::PingThiefToAllOfficer(class ASensorGadget* sensor, class AThief* thief)
{
	for (APlayerController* aPC : PC)
	{
		AGamePlayerController* playerController = Cast<AGamePlayerController>(aPC);
		AOfficer* officer = Cast<AOfficer>(playerController->GetPawn());
		if (officer == nullptr)
			continue;

		playerController->PingPlayerToSensor(sensor, thief);
	}
}

void AGameGameMode::stealMagnetCard(class AThief* thief, class AOfficer* officer)
{
	thief->MUlStealMagnetCard(officer);
}