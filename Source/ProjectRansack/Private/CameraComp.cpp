// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraComp.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UCameraComp::UCameraComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// Create a camera boom (pulls in towards the player if there is a collision)
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	
	cameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	cameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//camera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}


// Called when the game starts
void UCameraComp::BeginPlay()
{
	Super::BeginPlay();
	cameraBoom->SetupAttachment(GetOwner()->GetRootComponent());
	// ...
	
}


// Called every frame
void UCameraComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UCameraComp::SetupCamera(USceneComponent* root)
{
	cameraBoom->SetupAttachment(root);
	camera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
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

