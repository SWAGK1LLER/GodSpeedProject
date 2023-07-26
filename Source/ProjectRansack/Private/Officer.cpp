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
#include <Kismet/GameplayStatics.h>

AOfficer::AOfficer()
{
	bReplicates = true;
	flashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLight"));
	flashLight->SetupAttachment((USceneComponent*)cameraComponent->camera);

	sensorGadgetOfficer = CreateDefaultSubobject<USensorGadgetOfficerComponent>(TEXT("Sensor Gadget Component"));
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



void AOfficer::BeginPlay()
{
	Super::BeginPlay();
	if (!CheckTableInstance())
		return;

	CreateTimeline();
	flashLight->SetIntensity(0.f);
	sensorGadgetOfficer->ToggleEnable(false);
}

void AOfficer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MotionVisionTimeline.TickTimeline(DeltaTime);
	ChangeStencilOnMovement();

	if (usingSensorGadget)
		sensorGadgetOfficer->CalculateFirstPosition(this, cameraComponent->camera->GetComponentLocation(),
			cameraComponent->camera->GetForwardVector());
}

void AOfficer::TimelineProgress(float value)
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), officerTableInstance->MotionVisionMPC, "Scalar", value);
}

void AOfficer::TimelineFinished()
{
	MotionTimelineRunning = false;
}

void AOfficer::ChangeStencilOnMovement()
{
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

void AOfficer::CreateTimeline()
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

void AOfficer::HandleMotionVision()
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

void AOfficer::SensorGadgetAction()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SensorGadget!"));
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
	if (closeItems.Num() == 0)
		return;

	//Should use the camera forward to check wich item to use if multiple in array
	ItemUsing = closeItems[0];
	IOfficerInteractibleActor::Execute_Interact(ItemUsing->_getUObject(), this);
}

void AOfficer::StopInteract()
{
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
