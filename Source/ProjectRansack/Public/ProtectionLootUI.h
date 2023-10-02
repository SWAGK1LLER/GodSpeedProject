#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProtectionLootUI.generated.h"

UCLASS()
class PROJECTRANSACK_API UProtectionLootUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetDefaultText(const FString& msg);

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateProgressBar();

	UFUNCTION(BlueprintImplementableEvent)
	void setProgressBarValue(float percentage);

	FString getTextStateOfficer();

	FString getTextStateThief();
};
