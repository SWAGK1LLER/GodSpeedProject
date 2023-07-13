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

void AGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentTime = Timer * MINUTE;

	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance != nullptr)
	{
		//instance->StartGame();
	}

	startRound();
}

void AGameGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (RoundStarted)
		CurrentTime -= DeltaTime;

	FString timeRemaining = getRemainingTimeText();
	for (APlayerController* aPC : PC)
		(Cast<AGamePlayerController>(aPC))->ClientUpdateRoundTimeRemaining(timeRemaining);
}

void AGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PC.Add(NewPlayer);

	if (PC.Num() == MAX_PLAYER)
		startRound();
}

void AGameGameMode::SpawnPlayer(const ETeam& team, APlayerController* NewPlayer)
{
	APawn* pawn = NewPlayer->GetPawn();
	if (pawn != nullptr)
		pawn->K2_DestroyActor();

	FTransform spawnTransform;
	TSubclassOf<AActor> classToSpawn;

	FindSpawn(team, spawnTransform, classToSpawn);

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

	NewPlayer->Possess(actor);
}

void AGameGameMode::FindSpawn(const ETeam& team, FTransform& Location, TSubclassOf<AActor>& ActorClass)
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
		aPC->ClientUpdateTeamDuffleBagUI();
	}
}

void AGameGameMode::startRound()
{
	RoundStarted = true;
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