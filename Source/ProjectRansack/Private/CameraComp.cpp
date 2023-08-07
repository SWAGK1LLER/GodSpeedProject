// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraComp.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Base3C.h"
// Sets default values for this component's properties
UCameraComp::UCameraComp()
{
	PrimaryComponentTick.bCanEverTick = true;

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->bUsePawnControlRotation = true; 
}

void UCameraComp::SetupCamera(USceneComponent* root)
{
	camera->SetupAttachment(root, USpringArmComponent::SocketName);
}

void UCameraComp::SetupInputComponent(UInputComponent* PlayerInputComponent, UInputAction* Action)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &UCameraComp::look);
	}
}

void UCameraComp::fetchData(float pmaxPitchBottom, float pmaxPitchTop)
{
	maxPitchBottom = pmaxPitchBottom;
	maxPitchTop = pmaxPitchTop;
}

void UCameraComp::look(const FInputActionValue& Value)
{
	FVector2D Direction = Value.Get<FVector2D>();
	if (ACharacter* Character = CastChecked<ACharacter>(GetOwner()))
	{
		Character->AddControllerYawInput(Direction.X);
		float CurrentPitch = Character->GetControlRotation().Pitch;
		if (Direction.Y > 0 && (CurrentPitch < 270 || CurrentPitch > 360 - maxPitchTop))
			Character->AddControllerPitchInput(Direction.Y);
		else if(Direction.Y < 0 && (CurrentPitch > 90 || CurrentPitch < 0 + maxPitchBottom))
			Character->AddControllerPitchInput(Direction.Y);
	}
}

float UCameraComp::getFov()
{
	return camera->FieldOfView;
}

void UCameraComp::setFov(float newFov)
{
	camera->SetFieldOfView(newFov);
}

