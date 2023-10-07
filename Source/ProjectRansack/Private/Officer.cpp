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
#include "Equipement.h"
#include "Baton.h"
#include "Gun.h"

AOfficer::AOfficer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;

	cameraComponent = CreateDefaultSubobject<UCameraCompOfficer>(TEXT("Camera Component"));
	cameraComponent->SetupCamera(RootComponent);

	flashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLight"));
	flashLight->SetupAttachment((USceneComponent*)cameraComponent->camera);

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
	GetMesh()->SetOwnerNoSee(true); 
	SetupNotificationUI();

	StunArea->OnComponentBeginOverlap.AddDynamic(this, &AOfficer::OnStunTriggerOverlapBegin);
	StunArea->OnComponentEndOverlap.AddDynamic(this, &AOfficer::OnStunTriggerOverlapEnd);
	HelperClass::deactivateTrigger(StunArea);
	StunAreaActivate = false;

	equipement->StunWeapon->maxAmmo = equipement->StunWeapon->ammo = officerTableInstance->MaxGunAmmo;

	//LoadBelt
	UEOSGameInstance* instance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance != nullptr && instance->ServerGameSlot.saveGame != nullptr)
	{
		equipement->utilityBelt.Empty();
		equipement->utilityBelt.Add((EquipementPossibility)instance->GetPlayerSaveGame()->utilityBelt1Officer);
		equipement->utilityBelt.Add((EquipementPossibility)instance->GetPlayerSaveGame()->utilityBelt2Officer);
		equipement->utilityBelt.Add((EquipementPossibility)instance->GetPlayerSaveGame()->utilityBelt3Officer);

		if (WidgetUI != nullptr)
			WidgetUI->SetBeltImage(equipement->utilityBelt[0], equipement->utilityBelt[1], equipement->utilityBelt[2]);
	}
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
		EnhancedInputComponent->BindAction(officerTableInstance->StunBatonAction, ETriggerEvent::Started, this, &AOfficer::ToggleEquipStunBaton);
		EnhancedInputComponent->BindAction(officerTableInstance->ReloadAction, ETriggerEvent::Started, this, &AOfficer::ReloadGun);
	}
}

void AOfficer::SendDataToComponents()
{
	ABase3C::SendDataToComponents();

	equipement->sensorGadgetOfficer->fetchData(officerTableInstance->Range, officerTableInstance->RevealTime, officerTableInstance->MaxAmountOfSensors);
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

void AOfficer::ChangeStencilOnMovement()
{
}

void AOfficer::SetOfficerSensorScalor_Implementation(int newValue) //TODO Destroy officerSensor variable
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), officerTableInstance->MotionVisionMPC, "OfficerSensor", newValue);
}

void AOfficer::HandleMotionVision() //Reacts to the input of MotionVision
{
	if (isPaused)
		return;

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
	if (isPaused)
		return;

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
	if (isPaused)
		return;

	AGamePlayerController* playerController = Cast<AGamePlayerController>(GetController());
	if (playerController == nullptr)
		return;

	if (equipement->equippedWeapon == equipement->sensorGadgetOfficer)
	{
		IWeapon::Execute_UpdateUI(equipement->StunWeapon->_getUObject());
		//equipement->EquipWeapon(equipement->StunWeapon);
	}
	else
	{
		IWeapon::Execute_UpdateUI(equipement->sensorGadgetOfficer->_getUObject());
		//equipement->EquipWeapon(equipement->sensorGadgetOfficer);
	}

	playerController->SREquipWeapon(equipement, equipement->sensorGadgetOfficer);
	
}

void AOfficer::ToggleEquipStunBaton()
{
	if (isPaused)
		return;

	AGamePlayerController* playerController = Cast<AGamePlayerController>(GetController());
	if (playerController == nullptr)
		return;

	if (equipement->equippedWeapon == equipement->StunStick)
	{
		IWeapon::Execute_UpdateUI(equipement->StunWeapon->_getUObject());
		//equipement->EquipWeapon(equipement->StunWeapon);
	}
	else
	{
		IWeapon::Execute_UpdateUI(equipement->StunStick->_getUObject());
		//equipement->EquipWeapon(equipement->StunStick);
	}

	playerController->SREquipWeapon(equipement, equipement->StunStick);
}

void AOfficer::Fire()
{
	if (isPaused)
		return;

	if (bFreezeInput)
		return;

	if (ItemUsing != nullptr)
		return;

	AGamePlayerController* playerController = Cast<AGamePlayerController>(GetController());
	if (playerController == nullptr)
		return;

	playerController->SRFire(equipement);
}

void AOfficer::Interact()
{
	if (isPaused)
		return;

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

void AOfficer::ClientFreezeInput_Implementation(float duration, FVector DamageActorLocation)
{
	Super::ClientFreezeInput_Implementation(duration, DamageActorLocation);
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

void AOfficer::ReloadGun()
{
	if (equipement->equippedWeapon == equipement->StunWeapon && !equipement->StunWeapon->isFull())
		equipement->StunWeapon->Reload();
}