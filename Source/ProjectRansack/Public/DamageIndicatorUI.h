#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateColor.h"
#include "Math/Color.h"
#include "Engine/Texture2D.h"
#include "Math/Vector2D.h"
#include "DamageIndicatorUI.generated.h"

UCLASS()
class PROJECTRANSACK_API UDamageIndicatorUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowingDamage(bool bDynamic, bool bNearHit, float HideTime, float Angle, UTexture2D* Image, FVector2D Size, FSlateColor HitTint, float ImageOffset, FVector2D ScreenOffset);
};
