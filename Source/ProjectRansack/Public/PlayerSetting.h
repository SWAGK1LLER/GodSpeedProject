// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSetting.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UPlayerSetting : public USaveGame
{
	GENERATED_BODY()
public:

	//Security
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int SecuFov = 0;

	//Thief
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int ThiefFov = 0;

	//General
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int resolutionX = 1920;

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	int resolutionY = 1080;
};
