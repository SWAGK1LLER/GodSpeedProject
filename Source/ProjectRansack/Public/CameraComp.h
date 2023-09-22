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
	UCameraComp();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* camera;

	float maxPitchBottom;
	float maxPitchTop;

	virtual void SetupCamera(USceneComponent* root);

	void SetupInputComponent(class UInputComponent* PlayerInputComponent, class UInputAction* Action);
		
	void fetchData(float pmaxPitchBottom,float pmaxPitchTop);

	virtual void look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void setFov(float newFov);

	UFUNCTION(BlueprintCallable)
	float getFov();
};
