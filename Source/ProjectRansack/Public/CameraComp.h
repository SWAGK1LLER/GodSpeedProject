// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "CameraComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UCameraComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraComp();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* camera;

	float maxPitchBottom;
	float maxPitchTop;

	void SetupCamera(USceneComponent* root);

	void SetupInputComponent(class UInputComponent* PlayerInputComponent, class UInputAction* Action);
		
	void fetchData(float pmaxPitchBottom,float pmaxPitchTop);

	void look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void setFov(float newFov);

	UFUNCTION(BlueprintCallable)
	float getFov();
};
