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
	float SecuFov = 0;

	//Thief
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	float ThiefFov = 0;

	//Video
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	FString resolution = "1920X1080";

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	FString texture = "High";

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	FString shadow = "High";


	void InitializeSettings();

	int GetEnumNumber(const FString& pValue);
};
