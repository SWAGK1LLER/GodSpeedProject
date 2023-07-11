// Fill out your copyright notice in the Description page of Project Settings.


#include "Base3C.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthComponent.h"
#include "CameraComp.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ABase3C::ABase3C()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	cameraComponent = CreateDefaultSubobject<UCameraComp>(TEXT("Camera Component"));
	cameraComponent->SetupCamera(RootComponent);
	//Health Comp
	healthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void ABase3C::BeginPlay()
{
	Super::BeginPlay();

	BindInputHandler();
}

// Called every frame
void ABase3C::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABase3C::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Moving
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ABase3C::Move);
		EnhancedInputComponent->BindAction(interactAction, ETriggerEvent::Started, this, &ABase3C::Interact);
	}
	cameraComponent->SetupInputComponent(PlayerInputComponent, lookAction);
}


void ABase3C::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABase3C::Interact()
{
	TestDamage(this);
}

void ABase3C::TestDamage_Implementation(AActor* DamageActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Working!"));
	UGameplayStatics::ApplyDamage(DamageActor, 1, this->GetInstigatorController(), this, UDamageType::StaticClass());
}

void ABase3C::BindInputHandler()
{
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(inputHandler, 0);
}
