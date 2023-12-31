// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSaveGame.generated.h"


UENUM(BlueprintType)
enum EDefaultCharacter
{
	Thief,
	Officer
};

UCLASS()
class PROJECTRANSACK_API UPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int level = 1;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int xp = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int xpMax = 100;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	float xpCoefPerLevel = 0.25;

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int totalWin = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int totalLoose = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int timePlayThief = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int timePlaySecurity = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int totalArest = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int totalCash = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int totalExtraction = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int totalHightLoot = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int utilityBelt1Thief = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int utilityBelt2Thief = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int utilityBelt3Thief = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int utilityBelt1Officer = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int utilityBelt2Officer = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int utilityBelt3Officer = 0;


	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int defaultCharacter = EDefaultCharacter::Thief;

	UFUNCTION(BlueprintCallable)
	float GetPercent();

	void levelUp();
	void addXp(int pXp);
};
