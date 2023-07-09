// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayerController.h"
#include <Base3C.h>

void AGamePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	ABase3C* player = Cast<ABase3C>(InPawn);
	if (player != nullptr)
		player->BindInputHandler();

	PawnIsPossess(InPawn);
}