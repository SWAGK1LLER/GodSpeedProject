// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LightFuseBoxUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API ULightFuseBoxUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetDefaultText(const FString& msg);

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateProgressBar();

	UFUNCTION(BlueprintImplementableEvent)
	void setProgressBarValue(float percentage);

	FString getTextStateOfficer(bool lightOpen);

	FString getTextStateThief(bool hacked, bool lightOpen);
};
