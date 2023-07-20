// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSaveGame.generated.h"

/**
 * 
 */
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

	float GetPercent();
	void levelUp();
	void addXp(int pXp);
};
