// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EnumTeam.h"
#include "GameGameMode.generated.h"

#define MINUTE 60
#define SECOND 60
#define MAX_PLAYER 6

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API AGameGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Timer = 10;
	float CurrentTime = 0;
	bool RoundStarted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ScoreTeamA = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ScoreTeamB = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ThiefClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> OfficerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class APlayerController*> PC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABase3C*> TeamA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABase3C*> TeamB;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PostLogin(class APlayerController* NewPlayer) override;

	void SpawnPlayer(const ETeam& team, class APlayerController* NewPlayer);

	void FindSpawn(const ETeam& team, FTransform& Location, TSubclassOf<AActor>& ActorClass);

	void SendPlayerName();

	void SendTeamData();

	void UpdateTeamDuffleBag();

	UFUNCTION(BlueprintCallable)
	void startRound();

	UFUNCTION(BlueprintCallable)
	FString getRemainingTimeText();
	FString convertTimeToText();

	void AddToScore(int pValue, int& pScore);

	void FreezeInput(float duration, ABase3C* actor);
};
