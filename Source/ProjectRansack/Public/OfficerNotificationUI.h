// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OfficerNotificationUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UOfficerNotificationUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void PingOfficer(int CameraNumber);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void Reset();

};
