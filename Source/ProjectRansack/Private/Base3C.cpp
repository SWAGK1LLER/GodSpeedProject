// Fill out your copyright notice in the Description page of Project Settings.


#include "Base3C.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthComponent.h"
#include "CameraComp.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/CharacterMovementComponent.h"



// Sets default values
ABase3C::ABase3C()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	cameraComponent = CreateDefaultSubobject<UCameraComp>(TEXT("Camera Component"));
	cameraComponent->SetupCamera(RootComponent);
	//Health Comp
	healthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	
}

bool ABase3C::CheckTableInstance()
{
	if (tableInstance)
		return true;

	tableInstance = base3CDataTable->FindRow<FBase3CTable>(FName(TEXT("Base3C")), "");

	return tableInstance != nullptr;
}

void ABase3C::SendDataToComponents()
{
	CheckTableInstance();
	cameraComponent->fetchData(tableInstance->maxPitchBottom, tableInstance->maxPitchTop);
	healthComp->fetchData(tableInstance->currentHealth, tableInstance->maxHealth);
}

void ABase3C::SetClientNickname_Implementation(const FString& pNickName)
{
	MulticastSetClientNickname(pNickName);
	nickName = pNickName;
}

void ABase3C::MulticastSetClientNickname_Implementation(const FString& pNickName)
{
	nickName = pNickName;
}

// Called when the game starts or when spawned
void ABase3C::BeginPlay()
{
	Super::BeginPlay();
	CheckTableInstance();
	normalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed *= tableInstance->movementSpeed;

	BindInputHandler();

	tableInstance = base3CDataTable->FindRow<FBase3CTable>(FName(TEXT("Base3C")), "");

	if (tableInstance != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Working0"));
	}

	SendDataToComponents();

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
	
	CheckTableInstance();

	//Moving
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(tableInstance->moveAction, ETriggerEvent::Triggered, this, &ABase3C::Move);
		EnhancedInputComponent->BindAction(tableInstance->interactAction, ETriggerEvent::Started, this, &ABase3C::Interact);
		EnhancedInputComponent->BindAction(tableInstance->AimAction, ETriggerEvent::Triggered, this, &ABase3C::Aim);
		EnhancedInputComponent->BindAction(tableInstance->FireAction, ETriggerEvent::Started, this, &ABase3C::Fire);

		EnhancedInputComponent->BindAction(tableInstance->SprintAction, ETriggerEvent::Started, this, &ABase3C::Sprint);
		EnhancedInputComponent->BindAction(tableInstance->SprintAction, ETriggerEvent::Completed, this, &ABase3C::StopSprint);
	}
	cameraComponent->SetupInputComponent(PlayerInputComponent, tableInstance->lookAction);
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
		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABase3C::Interact()
{
	TestDamage(this);
}

void ABase3C::Aim()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Aiming!"));
}

void ABase3C::Fire()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Firing!"));
}

void ABase3C::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = normalWalkSpeed * tableInstance->sprintSpeed;
}

void ABase3C::TestDamage_Implementation(AActor* DamageActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Working!"));
	UGameplayStatics::ApplyDamage(DamageActor, 1, this->GetInstigatorController(), this, UDamageType::StaticClass());
}

void ABase3C::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = normalWalkSpeed * tableInstance->movementSpeed;
}

void ABase3C::BindInputHandler()
{
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(tableInstance->inputHandler, 0);
}
