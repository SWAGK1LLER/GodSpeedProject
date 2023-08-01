// Fill out your copyright notice in the Description page of Project Settings.


#include "Base3C.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CameraComp.h"
#include "StunWeapon.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/CharacterMovementComponent.h"
#include <EOSGameInstance.h>



// Sets default values
ABase3C::ABase3C()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	cameraComponent = CreateDefaultSubobject<UCameraComp>(TEXT("Camera Component"));
	cameraComponent->SetupCamera(RootComponent);
	
	currentState = CharacterState::Gun;

	StunWeapon = CreateDefaultSubobject<UStunWeapon>(TEXT("StunWeapon"));
}

// Called when the game starts or when spawned
void ABase3C::BeginPlay()
{
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();
	playTime = UGameplayStatics::GetRealTimeSeconds((GetWorld()));

	if(!CheckTableInstance())
		return;

	normalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed *= tableInstance->movementSpeed;

	BindInputHandler();
	SendDataToComponents();

}

// Called every frame
void ABase3C::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFreezeInput && FreezeDuration != -1)
	{
		TimeFreezed += DeltaTime;
		if (TimeFreezed >= FreezeDuration)
			UnFreezeInput();
	}		
}

void ABase3C::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason != EEndPlayReason::Type::Quit)
		return;

	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (playerController == nullptr)
		return;

	UEOSGameInstance* gameInstance = Cast<UEOSGameInstance>(GetGameInstance());
	if (gameInstance == nullptr)
		return;

	gameInstance->CloseGame();
	gameInstance->unregisterPlayerToGameSession(playerController);
}

bool ABase3C::CheckTableInstance()
{
	if (tableInstance)
		return true;

	if (!dataTable)
		return false;

	tableInstance = dataTable->FindRow<FBase3CTable>(FName(TEXT("Base3C")), "");

	return tableInstance != nullptr;
}

void ABase3C::SendDataToComponents()
{
	if (!CheckTableInstance())
		return;

	cameraComponent->fetchData(tableInstance->maxPitchBottom, tableInstance->maxPitchTop);
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

void ABase3C::ClientFreezeInput_Implementation(float duration)
{
	if (bFreezeInput)
		return;

	bFreezeInput = true;
	FreezeDuration = duration;
	TimeFreezed = 0;
}

void ABase3C::UnFreezeInput()
{
	bFreezeInput = false;
}

void ABase3C::SRReset_Implementation()
{
	MulReset();
}

void ABase3C::MulReset_Implementation()
{
	SetActorTransform(SpawnTransform);
	bFreezeInput = false;
	FreezeDuration = 0;
	TimeFreezed = 0;
}

// Called to bind functionality to input
void ABase3C::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(!CheckTableInstance())
		return;

	//Moving
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(tableInstance->moveAction, ETriggerEvent::Triggered, this, &ABase3C::Move);

		EnhancedInputComponent->BindAction(tableInstance->interactAction, ETriggerEvent::Started, this, &ABase3C::Interact);
		EnhancedInputComponent->BindAction(tableInstance->interactAction, ETriggerEvent::Completed, this, &ABase3C::StopInteract);

		EnhancedInputComponent->BindAction(tableInstance->AimAction, ETriggerEvent::Started, this, &ABase3C::StartAim);
		EnhancedInputComponent->BindAction(tableInstance->AimAction, ETriggerEvent::Completed, this, &ABase3C::StopAim);

		EnhancedInputComponent->BindAction(tableInstance->FireAction, ETriggerEvent::Started, this, &ABase3C::StartFire);
		EnhancedInputComponent->BindAction(tableInstance->FireAction, ETriggerEvent::Triggered, this, &ABase3C::Fire);

		EnhancedInputComponent->BindAction(tableInstance->SprintAction, ETriggerEvent::Started, this, &ABase3C::Sprint);
		EnhancedInputComponent->BindAction(tableInstance->SprintAction, ETriggerEvent::Completed, this, &ABase3C::StopSprint);


	}
	cameraComponent->SetupInputComponent(PlayerInputComponent, tableInstance->lookAction);
}


void ABase3C::Move(const FInputActionValue& Value)
{
	if (bFreezeInput)
		return;

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
	if (bFreezeInput)
		return;

	TestDamage(this);
}

void ABase3C::StopInteract()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Stop Interact!"));
}

void ABase3C::StartAim()
{
	if (bFreezeInput)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Aiming!"));
}

void ABase3C::StopAim()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("StopAiming!"));
}

void ABase3C::Fire()
{
	if (bFreezeInput || currentState != CharacterState::Gun)
		return;

	StunWeapon->Fire();

	//Ugly hack to trigger overlap event if actor is already in trigger volume
	TryGeneratingOverlapEvent();
}

void ABase3C::Sprint()
{
	if (bFreezeInput)
		return;

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
	if (tableInstance == nullptr)
		if (!CheckTableInstance())
			return;


	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(tableInstance->inputHandler, 0);
}

void ABase3C::ChangeStencilFromServer_Implementation(int pNewStencilValue) // this is multicast to tell everyone we are visible
{
	if (pNewStencilValue != 0)
		Revealed = true;
	else
		Revealed = false;

	GetMesh()->SetCustomDepthStencilValue(pNewStencilValue);
}

UCameraComp* ABase3C::GetCameraComponent()
{
	return cameraComponent;
}


void ABase3C::SetClientUI_Implementation()
{
	if (WidgetUI != nullptr)
		return;

	WidgetUI = CreateWidget<UPlayerUI>(GetWorld(), WidgetClass);
	WidgetUI->AddToViewport();
}

void ABase3C::TryGeneratingOverlapEvent()
{
	FVector location;
	location = GetActorLocation();

	location.X++;
	SetActorLocation(location);

	location.X--;
	SetActorLocation(location);
}