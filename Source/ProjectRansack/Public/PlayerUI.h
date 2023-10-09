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
	void ShowSensorEquipped();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowGrenade(class UTexture2D* uiTexture);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowStunBattonEquiped();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDecoy();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowCloakEquiped();

	UFUNCTION(BlueprintImplementableEvent)
	void SetBeltImage(int a, int b, int c);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDeguisementEquiped();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowEmpEquiped();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowClemore();
};
