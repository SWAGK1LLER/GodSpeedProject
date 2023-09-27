#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "UITabButton.generated.h"

UCLASS()
class PROJECTRANSACK_API UUITabButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FName tabId = "";
};
