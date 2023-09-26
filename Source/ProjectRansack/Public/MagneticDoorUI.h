#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MagneticDoorUI.generated.h"

UCLASS()
class PROJECTRANSACK_API UMagneticDoorUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetDefaultText(const FString& msg);

	FString getTextState(bool HasCard, bool doorOpen);
};
