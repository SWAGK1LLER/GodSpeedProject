// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectRansackGameMode.h"
#include "ProjectRansackCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectRansackGameMode::AProjectRansackGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
