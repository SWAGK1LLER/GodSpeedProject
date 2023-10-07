// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSaveGame.h"
#include "HelperClass.h"
#include <EOSGameInstance.h>
#include <Kismet/GameplayStatics.h>

float UPlayerSaveGame::GetPercent()
{
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

UPlayerSaveGame::~UPlayerSaveGame()
{
	bool destroy = true;
	return;
}