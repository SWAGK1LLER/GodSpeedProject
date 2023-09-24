#include "SecurityCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Officer.h"
#include "Thief.h"
#include "GameGameMode.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include <Net/UnrealNetwork.h>
#include <GamePlayerController.h>

#define RadToDegres 180 / PI

ASecurityCamera::ASecurityCamera()
{
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = true;
	spotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	spotLight->SetupAttachment(RootComponent);

	coneShape = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cone"));
	coneShape->SetupAttachment(spotLight);

	pingAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CameraPingAudio"));
	pingAudioComponent->bAutoActivate = false;
	pingAudioComponent->SetupAttachment(RootComponent);

	cameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("CameraMesh"));
	cameraMesh->SetupAttachment(spotLight);

	frozen = false;
}

void ASecurityCamera::BeginPlay()
{
	Super::BeginPlay();
	CheckDataTable();

	coneShape->OnComponentBeginOverlap.AddDynamic(this, &ASecurityCamera::OnTriggerOverlapBegin);
	coneShape->OnComponentEndOverlap.AddDynamic(this, &ASecurityCamera::OnTriggerOverlapEnd);

	if (pingAudioCue->IsValidLowLevelFast()) 
		pingAudioComponent->SetSound(pingAudioCue);

	CalculateBaseRotations();
}

void ASecurityCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckUndetectedThieves();
	if(!hasPinged)
		CheckAndRotate();
}

void ASecurityCamera::CheckDataTable()
{
	if (dataTable)
		tableInstance = dataTable->FindRow<FSecurityCameraDataTable>(FName(TEXT("SecurityCamera")), "");
}

void ASecurityCamera::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AThief>(OtherActor))
		UndetectedThieves.Add(OtherActor);
}

void ASecurityCamera::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(UndetectedThieves.Find(OtherActor) != INDEX_NONE)
		UndetectedThieves.Remove(OtherActor);
}

void ASecurityCamera::NotifyAllSecurity_Implementation(bool PingOrUnping)
{
	AGameGameMode* GameMode = (AGameGameMode*)GetWorld()->GetAuthGameMode();

	for (APlayerController* aPC : GameMode->PC)
	{
		AGamePlayerController* playerController = Cast<AGamePlayerController>(aPC);
		AOfficer* officer = Cast<AOfficer>(playerController->GetPawn());
		if (officer == nullptr)
			continue;
		
		if (PingOrUnping)
			officer->ReceiveCameraPing(cameraNumber);
		else
			officer->ReceiveCameraUnPing();
	}

	if (PingOrUnping)
	{
		if (!hasPinged)
			PlayAudio();

		hasPinged = true;
		spotLight->SetLightColor(tableInstance->ColorPing);

	}
	else
	{
		spotLight->SetLightColor(FLinearColor::White);
		hasPinged = false;
	}
}

void ASecurityCamera::PlayAudio_Implementation() //this probably works but I can't test the sound from one computer
{
	pingAudioComponent->Play();
}

void ASecurityCamera::CalculateBaseRotations()
{
	referenceForwardVector = GetActorForwardVector();
}

void ASecurityCamera::CheckAndRotate()
{
	if (frozen)
		return;

	if (rotationDirection == ERotationSide::Right)
	{
		FRotator newRotation = GetActorForwardVector().RotateAngleAxis(tableInstance->rotationSpeed, GetActorUpVector()).Rotation();
		SetActorRotation(newRotation);
		float Angle = -acos(FVector::DotProduct(referenceForwardVector, GetActorForwardVector())) * RadToDegres;

		if (Angle < -tableInstance->rotateAmount)
			rotationDirection = ERotationSide::Left;
	}
	else if (rotationDirection == ERotationSide::Left)
	{
		FRotator newRotation = GetActorForwardVector().RotateAngleAxis(-tableInstance->rotationSpeed, GetActorUpVector()).Rotation();
		SetActorRotation(newRotation);
		float Angle = acos(FVector::DotProduct(referenceForwardVector, GetActorForwardVector())) * RadToDegres;

		if (Angle > tableInstance->rotateAmount)
			rotationDirection = ERotationSide::Right;
	}
}

void ASecurityCamera::CheckUndetectedThieves()
{
	if (!frozen)
	{
		bool hasDetected = false;
		FHitResult Hit(ForceInit);
		FVector Start = GetActorLocation();
		FVector End;
		FCollisionQueryParams CollisionParams;

		CollisionParams.AddIgnoredActor(this);

		for (AActor* Thief : UndetectedThieves)
		{
			End = Thief->GetActorLocation();
			GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldDynamic, CollisionParams);

			if (Hit.GetActor() == Thief)
			{
				if(!hasPinged)
					NotifyAllSecurity(true);

				hasDetected = true;
			}
		}
		if (!hasDetected)
		{
			if (hasPinged)
				NotifyAllSecurity(false);
		}
	}
	else
	{
		if (hasPinged)
			NotifyAllSecurity(false);
	}
}

void ASecurityCamera::Mul_FreezeCamera_Implementation()
{
	if (frozen)
		return;

	frozen = true;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this] {
		frozen = false;
	}), tableInstance->timeToUnfreeze, false);
}