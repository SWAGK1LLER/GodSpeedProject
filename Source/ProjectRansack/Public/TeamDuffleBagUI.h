// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory.h"
#include "TeamDuffleBagUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRANSACK_API UTeamDuffleBagUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUIBagName1(const FString& nickname);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUIBagName2(const FString& nickname);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUIBag1(const FString& nickname, const TArray<FItemLooted>& Items);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUIBag2(const FString& nickname, const TArray<FItemLooted>& Items);

	UFUNCTION(BlueprintImplementableEvent)
	void ToogleView(bool bShow);
};
