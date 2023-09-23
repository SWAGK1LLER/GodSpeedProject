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
	FRotator finalRotation;
	bool ShouldTurn = false;
	float speed = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	float SnapSpeed = 0.4f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	float SnapThreshold = 0.65f;

	USceneComponent* rootMesh = nullptr;

	UCameraCompThief();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void look(const FInputActionValue& Value) override;
	void SetupCamera(USceneComponent* root) override;

	void RotatePlayer(class ACharacter* Character);
};
