#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StunOfficerUI.generated.h"

UCLASS()
class PROJECTRANSACK_API UStunOfficerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void NoCardToSteal();
};
