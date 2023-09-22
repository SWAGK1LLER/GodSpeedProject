#pragma once

#include "CoreMinimal.h"
#include "CameraComp.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraCompThief.generated.h"

UCLASS()
class PROJECTRANSACK_API UCameraCompThief : public UCameraComp
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom = nullptr;

	UCameraCompThief();
	void look(const FInputActionValue& Value) override;
	void SetupCamera(USceneComponent* root) override;
};
