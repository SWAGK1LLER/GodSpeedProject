// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractibleActor.h"
#include "OfficerInteractibleActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOfficerInteractibleActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTRANSACK_API IOfficerInteractibleActor : public IInteractibleActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(class AActor* pActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StopInteract(class AActor* pActor);
};
