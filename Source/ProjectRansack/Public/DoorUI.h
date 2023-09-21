// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DoorUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UDoorUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetDefaultText(const FString& msg);

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateProgressBar();

	UFUNCTION(BlueprintImplementableEvent)
	void setProgressBarValue(float percentage);

	FString getTextStateOfficer(bool hacked, bool doorOpen);

	FString getTextStateThief(bool hacked, bool doorOpen);
};
