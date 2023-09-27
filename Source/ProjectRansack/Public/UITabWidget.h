#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UITabWidget.generated.h"

UCLASS()
class PROJECTRANSACK_API UUITabWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUITabList* tab = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* switcher = nullptr;
};
