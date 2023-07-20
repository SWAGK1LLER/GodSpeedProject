// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSaveGame.h"
#include "HelperClass.h"

float UPlayerSaveGame::GetPercent()
{
	UE_LOG(LogTemp, Warning, TEXT("level %d"), level);
	UE_LOG(LogTemp, Warning, TEXT("xp %d"), xp);
	UE_LOG(LogTemp, Warning, TEXT("xpMax %d"), xpMax);
	UE_LOG(LogTemp, Warning, TEXT("xpCoefPerLevel %d"), xpCoefPerLevel);

	return HelperClass::mapValue(xp, 0, xpMax, 0, 1);
}

void UPlayerSaveGame::levelUp()
{
	level++;
	xp -= xpMax;
	xpMax += xpMax * xpCoefPerLevel;
}

void UPlayerSaveGame::addXp(int pXp)
{
	xp += pXp;
	while (xp >= xpMax)
		levelUp();
}