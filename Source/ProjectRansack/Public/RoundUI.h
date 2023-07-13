// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API URoundUI : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent)
	void SetTime(const FString& time);
};
