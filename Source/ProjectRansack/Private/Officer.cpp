// Fill out your copyright notice in the Description page of Project Settings.


#include "Officer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/TimelineComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Runtime/Engine/Classes/Kismet/KismetMaterialLibrary.h"
#include "Components/SpotLightComponent.h"
#include "CameraComp.h"
#include "SensorGadget.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "SensorGadgetOfficerComponent.h"
#include <GameGameMode.h>
#include "ArrestUI.h"
#include <Kismet/GameplayStatics.h>
#include <GamePlayerController.h>
#include <HelperClass.h>

AOfficer::AOfficer()
{
	bReplicates = true;
	flashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLight"));
	flashLight->SetupAttachment((USceneComponent*)cameraComponent->camera);

	sensorGadgetOfficer = CreateDefaultSubobject<USensorGadgetOfficerComponent>(TEXT("Sensor Gadget Component"));
}

void AOfficer::BeginPlay()
{
	Super::BeginPlay();
	if (!CheckTableInstance())
		return;

	CreateTimeline();
	SendDataToComponents();
	flashLight->SetIntensity(0.f);
	sensorGadgetOfficer->ToggleEnable(false);
	GetMesh()->SetOwnerNoSee(true); 
	SetupNotificationUI();
}


void AOfficer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!CheckTableInstance())
		return;

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(officerTableInstance->MotionVisionAction, ETriggerEvent::Started, this, &AOfficer::HandleMotionVision);
		EnhancedInputComponent->BindAction(officerTableInstance->FlashlightAction, ETriggerEvent::Started, this, &AOfficer::ToggleFlashight);
		EnhancedInputComponent->BindAction(officerTableInstance->SensorGadgetAction, ETriggerEvent::Started, this, &AOfficer::SensorGadgetAction);
	}
}

void AOfficer::SendDataToComponents()
{
	sensorGadgetOfficer->fetchData(officerTableInstance->Range, officerTableInstance->RevealTime, officerTableInstance->MaxAngle, officerTableInstance->MaxAmountOfSensors);
}

void AOfficer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MotionVisionTimeline.TickTimeline(DeltaTime);
	ChangeStencilOnMovement();

	if (startArrest)
	{
		arrestTime += DeltaTime;

		AGamePlayerController* playerController = Cast<AGamePlayerController>(GetController());
		if (playerController == nullptr)
			return;

		if (ArrestingThief == nullptr)
		{
			startArrest = false;
			return;
		}

		if (arrestTime >= TimeToArrestThief)
		{
			ArrestThief(ArrestingThief);
			startArrest = false;
			ArrestingThief = nullptr;
		}
		else
		{
			UArrestUI* ui = Cast<UArrestUI>(playerController->GetWidget(ArrestingThief));
			ui->ShowProgress();
			ui->SetProgress(HelperClass::mapValue(arrestTime, 0, TimeToArrestThief, 0, 1));
		}
	}


	if (usingSensorGadget)
		sensorGadgetOfficer->CalculateFirstPosition(this, cameraComponent->camera->GetComponentLocation(),
			cameraComponent->camera->GetForwardVector());
}

void AOfficer::MulReset_Implementation()
{
	Super::MulReset_Implementation();
}

void AOfficer::TimelineProgress(float value)
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), officerTableInstance->MotionVisionMPC, "Scalar", value);
}

void AOfficer::TimelineFinished()
{
	MotionTimelineRunning = false;
}

void AOfficer::ChangeStencilOnMovement() //Reveals enemies when motion vision is active
{
	if (Revealed)
	{
		return;
	}
		

	if (GetVelocity().Length() > 0)
	{
		if (GetMesh())
			GetMesh()->SetCustomDepthStencilValue(officerTableInstance->MotionSensorStencilBufNumber);
	}
	else
	{
		if (GetMesh())
			GetMesh()->SetCustomDepthStencilValue(0);
	}
}

void AOfficer::SetOfficerSensorScalor_Implementation(int newValue) //TODO Destroy officerSensor variable
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), officerTableInstance->MotionVisionMPC, "OfficerSensor", newValue);
}

void AOfficer::CreateTimeline() //Timeline is used for the motion vision to change between states
{

	if (officerTableInstance->MotionVisionFloatCurve)
	{
		FOnTimelineFloat TimelineProgress;
		FOnTimelineEventStatic onTimelineFinishedCallback;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		MotionVisionTimeline.AddInterpFloat(officerTableInstance->MotionVisionFloatCurve, TimelineProgress);
		MotionVisionTimeline.SetLooping(false);
		onTimelineFinishedCallback.BindUFunction(this, FName("TimelineFinished"));
		MotionVisionTimeline.SetTimelineFinishedFunc(onTimelineFinishedCallback);
	}
}



bool AOfficer::CheckTableInstance()
{
	if (officerTableInstance)
		return true;

	if (!officerDataTable)
		return false;

	officerTableInstance = officerDataTable->FindRow<FOfficerTable>(FName(TEXT("Officer")), "");

	return officerTableInstance != nullptr;
}

void AOfficer::HandleMotionVision() //Reacts to the input of MotionVision
{
	if (MotionTimelineRunning)
		return;

	MotionTimelineRunning = true;
	
	if (!motionSensorActive)
	{
		MotionVisionTimeline.PlayFromStart();
		motionSensorActive = true;
	}
	else
	{
		MotionVisionTimeline.ReverseFromEnd();
		motionSensorActive = false;
	}
}

void AOfficer::ToggleFlashight()
{
	if (flashLightOn)
	{
		flashLight->SetIntensity(0.f);
		flashLightOn = false;
	}
	else
	{
		flashLight->SetIntensity(5000.f); 
		flashLightOn = true;
	}
		
}

void AOfficer::SensorGadgetAction() //Reacts to the input of SensorGadget
{
	if (currentState == CharacterState::SensorGadget)
	{
		currentState = CharacterState::Gun;
		usingSensorGadget = false;
		sensorGadgetOfficer->ToggleEnable(false);
		return;
	}

	currentState = CharacterState::SensorGadget;
	sensorGadgetOfficer->ToggleEnable(true);
	usingSensorGadget = true;
	
}

void AOfficer::StartFire()
{
	Super::StartFire();
	if (currentState == CharacterState::SensorGadget)
	{
		sensorGadgetOfficer->TryPlace();
	}
}

void AOfficer::Interact()
{
	Super::Interact();
	if (closeThief.Num() != 0)
	{
		startArrest = true;
		arrestTime = 0;
		ArrestingThief = closeThief[0];
		return;
	}

	if (closeItems.Num() == 0)
		return;

	//Should use the camera forward to check wich item to use if multiple in array
	ItemUsing = closeItems[0];
	IOfficerInteractibleActor::Execute_Interact(ItemUsing->_getUObject(), this);
}

void AOfficer::StopInteract()
{
	if (ArrestingThief != nullptr)
	{
		AGamePlayerController* playerController = Cast<AGamePlayerController>(GetController());
		if (playerController == nullptr)
			return;

		UArrestUI* ui = Cast<UArrestUI>(playerController->GetWidget(ArrestingThief));
		ui->Reset();

		startArrest = false;
		ArrestingThief = nullptr;
	}

	

	if (ItemUsing == nullptr)
		return;

	IOfficerInteractibleActor::Execute_StopInteract(ItemUsing->_getUObject(), this);
	ItemUsing = nullptr;
}

void AOfficer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AOfficer::ArrestThief_Implementation(ABase3C* other)
{
	AGameGameMode* gameMode = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != nullptr)
		gameMode->ArrestThief(other);
}


void AOfficer::SetupNotificationUI()
{
	if (notificationUI != nullptr)
		return;

	notificationUI = CreateWidget<UOfficerNotificationUI>(GetWorld(), notificationWidgetClass);
	notificationUI->AddToViewport();
}

void AOfficer::ReceiveCameraPing_Implementation(int CameraNumb)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CAMERA PINGED!"));
	notificationUI->PingOfficer(CameraNumb);
}


void AOfficer::ReceiveCameraUnPing_Implementation()
{
	notificationUI->Reset();
}