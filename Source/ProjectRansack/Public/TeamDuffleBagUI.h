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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UGridPanel* gridTeamMate1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UGridPanel* gridTeamMate2 = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUIBagName1(const FString& nickname);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUIBagName2(const FString& nickname);

	void UpdateUITeamMate1(const FString& nickname, const TArray<FItemLooted>& Items);

	void UpdateUITeamMate2(const FString& nickname, const TArray<FItemLooted>& Items);

	UFUNCTION(BlueprintImplementableEvent)
	void ToogleView(bool bShow);
};
