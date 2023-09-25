#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlidingDoorUI.generated.h"

UCLASS()
class PROJECTRANSACK_API USlidingDoorUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetDefaultText(const FString& msg);

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateProgressBar();

	UFUNCTION(BlueprintImplementableEvent)
	void setProgressBarValue(float percentage);

	FString getTextStateOfficer(bool hacked, bool doorOpen);

	FString getTextStateThief(bool hacked, bool doorOpen);
};
