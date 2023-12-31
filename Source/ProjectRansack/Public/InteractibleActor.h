// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractibleActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractibleActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTRANSACK_API IInteractibleActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(class AActor* pActor); 

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StopInteract(class AActor* pActor);
};
