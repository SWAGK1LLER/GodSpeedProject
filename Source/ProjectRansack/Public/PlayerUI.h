#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUI.generated.h"

UCLASS()
class PROJECTRANSACK_API UPlayerUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCooldown(float time);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowGunReady();

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleMagnetCard(bool show);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowGunEquipped();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowStunBattonEquiped();
};
