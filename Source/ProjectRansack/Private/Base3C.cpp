#include "Base3C.h"
#include "Thief.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CameraComp.h"
#include "StunWeapon.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/CharacterMovementComponent.h"
#include <EOSGameInstance.h>
#include <HelperClass.h>
#include "DamageIndicatorComp.h"
#include "GamePlayerController.h"
#include "Components/AudioComponent.h"
#include "Equipement.h"
#include "GrenadeTrajectory.h"

ABase3C::ABase3C(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	equipement = CreateDefaultSubobject<UEquipement>(TEXT("equipement"));
	equipement->RegisterComponent();

	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));

	damageIndicator = CreateDefaultSubobject<UDamageIndicatorComp>(TEXT("DamageIndicator"));
}

void ABase3C::BeginPlay()
{
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();
	equipement->FinishAttachement(GetMesh());

	if(!CreateTableInstance())
		return;

	normalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed *= tableInstance->movementSpeed;

	BindInputHandler();
	SendDataToComponents();
}

void ABase3C::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFreezeInput && FreezeDuration != -1)
	{
		TimeFreezed += DeltaTime;
		if (TimeFreezed >= FreezeDuration)
			UnFreezeInput();
	}

	if (bIsZooming)
	{
		//Zoom
		if (zoomTime < tableInstance->aimZoomSpeed)
		{
			zoomTime += DeltaTime;
			zoomTime = FMath::Clamp(zoomTime, zoomTime, tableInstance->aimZoomSpeed);

			float alpha = HelperClass::mapValue(zoomTime, 0, tableInstance->aimZoomSpeed, 0, 1);
			float zoom = -FMath::Lerp(0, tableInstance->aimZoom, alpha);
			cameraComponent->camera->ClearAdditiveOffset();
			cameraComponent->camera->AddAdditiveOffset(FTransform(), zoom);
		}
	}
	else if (zoomTime > 0)
	{
		//Un zoom
		zoomTime -= DeltaTime;
		zoomTime = FMath::Clamp(zoomTime, 0, zoomTime);

		float alpha = HelperClass::mapValue(zoomTime, 0, tableInstance->aimZoomSpeed, 0, 1);
		float zoom = -FMath::Lerp(0, tableInstance->aimZoom, alpha);
		cameraComponent->camera->ClearAdditiveOffset();
		cameraComponent->camera->AddAdditiveOffset(FTransform(), zoom);
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

bool ABase3C::CreateTableInstance()
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
	if (!CreateTableInstance())
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

void ABase3C::ClientFreezeInput_Implementation(float duration, FVector DamageActorLocation)
{
	if (GetController() != nullptr && GetController()->IsLocalPlayerController())
		damageIndicator->ShowDamage(DamageActorLocation);

	// Can not freeze again someone already freeze to reset count down
	if (bFreezeInput)
		return;

	bFreezeInput = true;
	FreezeDuration = duration;
	TimeFreezed = 0;
	StopInteract();
	StopAim();
}

void ABase3C::UnFreezeInput_Implementation()
{
	bFreezeInput = false;
}

void ABase3C::SRReset_Implementation()
{
	MulReset(SpawnTransform);
}

void ABase3C::MulReset_Implementation(FTransform transform)
{
	SetActorTransform(transform);
	bFreezeInput = false;
	FreezeDuration = 0;
	TimeFreezed = 0;
}

// Called to bind functionality to input
void ABase3C::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(!CreateTableInstance())
		return;

	//Moving
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(tableInstance->moveAction, ETriggerEvent::Triggered, this, &ABase3C::Move);

		EnhancedInputComponent->BindAction(tableInstance->interactAction, ETriggerEvent::Started, this, &ABase3C::Interact);
		EnhancedInputComponent->BindAction(tableInstance->interactAction, ETriggerEvent::Completed, this, &ABase3C::StopInteract);

		EnhancedInputComponent->BindAction(tableInstance->AimAction, ETriggerEvent::Started, this, &ABase3C::StartAim);
		EnhancedInputComponent->BindAction(tableInstance->AimAction, ETriggerEvent::Completed, this, &ABase3C::StopAim);

		EnhancedInputComponent->BindAction(tableInstance->FireAction, ETriggerEvent::Started, this, &ABase3C::Fire);

		EnhancedInputComponent->BindAction(tableInstance->SprintAction, ETriggerEvent::Started, this, &ABase3C::SRStartSprinting);
		EnhancedInputComponent->BindAction(tableInstance->SprintAction, ETriggerEvent::Completed, this, &ABase3C::SRStopSprinting);

		EnhancedInputComponent->BindAction(tableInstance->TabAction, ETriggerEvent::Started, this, &ABase3C::Tab);
		EnhancedInputComponent->BindAction(tableInstance->TabAction, ETriggerEvent::Completed, this, &ABase3C::StopTab);
		
		EnhancedInputComponent->BindAction(tableInstance->PauseAction, ETriggerEvent::Started, this, &ABase3C::TogglePauseMenu);

		equipement->BindBeltKey(EnhancedInputComponent, tableInstance);
	}
	
	cameraComponent->SetupInputComponent(PlayerInputComponent, tableInstance->lookAction);
}

void ABase3C::Move(const FInputActionValue& Value)
{
	if (isPaused)
		return;

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
	if (isPaused)
		return;

	if (bFreezeInput)
		return;
}

void ABase3C::StopInteract()
{
}

void ABase3C::StartAim()
{
	if (isPaused)
		return;

	if (bFreezeInput)
		return;

	bIsZooming = true;
}

void ABase3C::StopAim()
{
	bIsZooming = false;
}

void ABase3C::Fire()
{
	if (isPaused)
		return;

	if (bFreezeInput)
		return;

	AGamePlayerController* playerController = Cast<AGamePlayerController>(GetController());
	if (playerController == nullptr)
		return;

	playerController->SRFire(equipement);
}

void ABase3C::SRStartSprinting_Implementation()
{
	MulStartSprinting();
}

void ABase3C::MulStartSprinting_Implementation()
{
	if (bFreezeInput)
		return;

	GetCharacterMovement()->MaxWalkSpeed = normalWalkSpeed * tableInstance->sprintSpeed;
	bIsRunning = true;
}

void ABase3C::SRStopSprinting_Implementation()
{
	MulStopSprinting();
}

void ABase3C::MulStopSprinting_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = normalWalkSpeed * tableInstance->movementSpeed;
	bIsRunning = false;
}

void ABase3C::Sprint()
{
	if (bFreezeInput)
		return;

	GetCharacterMovement()->MaxWalkSpeed = normalWalkSpeed * tableInstance->sprintSpeed;
}

void ABase3C::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = normalWalkSpeed * tableInstance->movementSpeed;
}

void ABase3C::Tab()
{
	if (isPaused)
		return;
}

void ABase3C::StopTab()
{
	
}

void ABase3C::BindInputHandler()
{
	if (tableInstance == nullptr)
		if (!CreateTableInstance())
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

	if (IsA(AThief::StaticClass()))
		WidgetUI->ToggleMagnetCard(false);

	WidgetUI->SetBeltImage(equipement->utilityBelt[0], equipement->utilityBelt[1], equipement->utilityBelt[2]);
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

void ABase3C::ToggleMagnetCardUI_Implementation(bool visible)
{
	if (WidgetUI != nullptr)
		WidgetUI->ToggleMagnetCard(visible);
}

void ABase3C::TogglePauseMenu()
{
	Cast<AGamePlayerController>(GetController())->TogglePauseMenu();
}

void ABase3C::PlayFootstep()
{
	if (!bIsRunning)
		return;

	audioComp->Play();
}

void ABase3C::MUlThrowGrenade_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance->Montage_IsPlaying(GrenadeThrowMontage))
		return;

	AnimInstance->Montage_Play(GrenadeThrowMontage);
}

void ABase3C::ThrowGrenade()
{
	equipement->GrenateTrajectory->ThrowGrenade();
}

void ABase3C::ThrowFinish()
{
	equipement->GrenateTrajectory->EndThrow();
}