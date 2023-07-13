// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractibleActor.h"
#include "ThiefInteractibleActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UThiefInteractibleActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTRANSACK_API IThiefInteractibleActor : public IInteractibleActor
{
	GENERATED_BODY()

public:
};
