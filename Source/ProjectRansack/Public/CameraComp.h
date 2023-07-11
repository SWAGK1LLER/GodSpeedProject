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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* cameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* camera;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = CameraParameters, meta = (AllowPrivateAccess = "true"))
		float MaxPitchBottom = 30;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = CameraParameters, meta = (AllowPrivateAccess = "true"))
		float MaxPitchTop = 30;

	void SetupCamera(USceneComponent* root);

	void SetupInputComponent(class UInputComponent* PlayerInputComponent, class UInputAction* Action);
		

	void look(const FInputActionValue& Value);
};