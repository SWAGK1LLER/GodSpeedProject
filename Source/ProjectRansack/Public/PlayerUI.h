// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UPlayerUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateRemainingAmmo(int ammo);

	UFUNCTION(BlueprintImplementableEvent)
	void SetMaxAmmo(int maxAmmo);
};