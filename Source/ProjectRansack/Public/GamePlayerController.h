// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Inventory.h"
#include "GamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateInventoryUI(const TArray<FItemLooted>& pItems);

	virtual void SetPawn(APawn* InPawn) override;

	UFUNCTION(BlueprintImplementableEvent)
	void PawnIsPossess(APawn* InPawn);
};
