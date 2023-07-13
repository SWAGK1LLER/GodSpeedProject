// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory.h"
#include "DuffleBagUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UDuffleBagUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUI(const TArray<FItemLooted>& Items);
};
