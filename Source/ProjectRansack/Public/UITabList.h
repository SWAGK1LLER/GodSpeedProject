#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "Components/HorizontalBox.h"
#include "UITabList.generated.h"

UCLASS()
class PROJECTRANSACK_API UUITabList : public UCommonTabListWidgetBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* content = nullptr;

	void HandleTabCreation(FName TabNameID, class UCommonButtonBase* TabButton);
	void HandleTabRemoval(FName TabNameID, class UCommonButtonBase* TabButton);
};
