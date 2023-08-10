// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExtractionZoneUI.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTRANSACK_API UExtractionZoneUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void setProgressBarValue(float percentage);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowProgressBar();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowEmptyInventory();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowThiefFreezed();
};
