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
#include "CameraCompOfficer.h"
#include <Components/BoxComponent.h>

AOfficer::AOfficer()
{
	bReplicates = true;

	cameraComponent = CreateDefaultSubobject<UCameraCompOfficer>(TEXT("Camera Component"));
	cameraComponent->SetupCamera(RootComponent);

	flashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLight"));
	flashLight->SetupAttachment((USceneComponent*)cameraComponent->camera);

	sensorGadgetOfficer = CreateDefaultSubobject<USensorGadgetOfficerComponent>(TEXT("Sensor Gadget Component"));

	HasMagnetCard = true;

	StunArea = CreateDefaultSubobject<UBoxComponent>(FName("StunArea"));
	StunArea->SetGenerateOverlapEvents(true);
	StunArea->SetupAttachment(RootComponent);
}

void AOfficer::BeginPlay()
{
	Super::BeginPlay();
	if (!CreateTableInstance())
		return;

	CreateTimeline();
	SendDataToComponents();
	flashLight->SetIntensity(0.f);
	sensorGadgetOfficer->ToggleEnable(false);
	GetMesh()->SetOwnerNoSee(true); 
	SetupNotificationUI();

	StunArea->OnComponentBeginOverlap.AddDynamic(this, &AOfficer::OnStunTriggerOverlapBegin);
	StunArea->OnComponentEndOverlap.AddDynamic(this, &AOfficer::OnStunTriggerOverlapEnd);
	HelperClass::deactivateTrigger(StunArea);
	StunAreaActivate = false;
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
		sensorGadgetOfficer->updatePosing(cameraComponent->camera->GetComponentLocation(), cameraComponent->camera->GetForwardVector());
}

void AOfficer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!CreateTableInstance())
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
	ABase3C::SendDataToComponents();
	sensorGadgetOfficer->fetchData(officerTableInstance->Range, officerTableInstance->RevealTime, officerTableInstance->MaxAmountOfSensors);
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

bool AOfficer::CreateTableInstance()
{
	ABase3C::CreateTableInstance();

	if (officerTableInstance)
		return true;

	if (!officerDataTable)
		return false;

	officerTableInstance = officerDataTable->FindRow<FOfficerTable>(FName(TEXT("Officer")), "");

	return officerTableInstance != nullptr;
}

void AOfficer::TimelineProgress(float value)
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), officerTableInstance->MotionVisionMPC, "Scalar", value);
}

void AOfficer::TimelineFinished()
{
	MotionTimelineRunning = false;
}

void AOfficer::MulReset_Implementation(FTransform transform)
{
	Super::MulReset_Implementation(transform);
}

void AOfficer::ChangeStencilOnMovement() //Reveals enemies when motion vision is active
{
	/*if (Revealed)
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
	}*/
}

void AOfficer::SetOfficerSensorScalor_Implementation(int newValue) //TODO Destroy officerSensor variable
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), officerTableInstance->MotionVisionMPC, "OfficerSensor", newValue);
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
		sensorGadgetOfficer->Place();
	}
}

void AOfficer::Interact()
{
	if (bFreezeInput)
		return;

	if (closeThief.Num() != 0)
	{
		if (((AThief*)(closeThief[0]))->beingArrest)
			return;
		
		startArrest = true;
		arrestTime = 0;
		ArrestingThief = closeThief[0];

		AGamePlayerController* playerController = Cast<AGamePlayerController>(GetController());
		if (playerController == nullptr)
			return;

		playerController->SRBeginArrestThief((AThief*)ArrestingThief, true, this);
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

		playerController->SRBeginArrestThief((AThief*)ArrestingThief, false, nullptr);
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
	if(notificationUI)
		notificationUI->PingOfficer(CameraNumb);
}


void AOfficer::ReceiveCameraUnPing_Implementation()
{
	notificationUI->Reset();
}

void AOfficer::OnStunTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AThief* player = Cast<AThief>(OtherActor);
	if (player == nullptr)
		return;

	player->closeOfficer.Add(this);

	AController* PC = player->GetController();
	if (PC != nullptr && PC->IsLocalPlayerController())
	{
		AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
		UStunOfficerUI* ui = (UStunOfficerUI*)playerController->AddInteractibleWidgetUI(this, StunOfficerWidgetClass);

		if (!HasMagnetCard || player->HasMagnetCard)
			ui->NoCardToSteal();
	}
}

void AOfficer::OnStunTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AThief* player = Cast<AThief>(OtherActor);
	if (player == nullptr)
		return;

	player->closeOfficer.Remove(this);

	AController* PC = player->GetController();
	if (PC != nullptr && PC->IsLocalPlayerController())
	{
		AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
		playerController->RemoveInteractibleWidgetUI(this);
	}
}

void AOfficer::ClientFreezeInput_Implementation(float duration, AActor* pActor)
{
	Super::ClientFreezeInput_Implementation(duration, pActor);
	SRActivateStunTrigger();
}

void AOfficer::SRActivateStunTrigger_Implementation()
{
	MulActivateStunTrigger();
}

void AOfficer::MulActivateStunTrigger_Implementation()
{
	HelperClass::activateTrigger(StunArea);
	StunAreaActivate = true;
}

void AOfficer::UnFreezeInput_Implementation()
{
	Super::UnFreezeInput_Implementation();
	HelperClass::deactivateTrigger(StunArea);
	StunAreaActivate = false;
}

void AOfficer::ToggleMagnetCard_Implementation(bool possess)
{
	HasMagnetCard = possess;
	ToggleMagnetCardIU();
}

void AOfficer::ToggleMagnetCardIU_Implementation()
{
	ToggleMagnetCardUI(HasMagnetCard);
}