// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArrestUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UArrestUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetProgress(float progress);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowProgress();

	UFUNCTION(BlueprintImplementableEvent)
	void Reset();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowAlreadyArresting();

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleBeingArrested(bool isArrested);
};
