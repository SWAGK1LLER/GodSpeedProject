#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePauseMenu.generated.h"

UCLASS()
class PROJECTRANSACK_API UGamePauseMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleView(bool visible);
};
