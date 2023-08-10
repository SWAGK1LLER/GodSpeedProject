// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TerminalUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UTerminalUI : public UUserWidget
{
public:
	GENERATED_BODY()
		UFUNCTION(BlueprintImplementableEvent)
		void SetDefaultText(const FString& msg);

	UFUNCTION(BlueprintImplementableEvent)
		void ActivateProgressBar();

	UFUNCTION(BlueprintImplementableEvent)
		void setProgressBarValue(float percentage);

	FString getTextStateOfficer(bool hacked, bool CamerasOff);

	FString getTextStateThief(bool hacked, bool CamerasOff);

};
