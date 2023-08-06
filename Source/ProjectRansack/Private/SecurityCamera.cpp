// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Officer.h"
#include "Thief.h"
#include "GameGameMode.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include <Net/UnrealNetwork.h>

ASecurityCamera::ASecurityCamera()
{
	bReplicates = true;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

// Called when the game starts or when spawned
void ASecurityCamera::BeginPlay()
{
	Super::BeginPlay();

	coneShape->OnComponentBeginOverlap.AddDynamic(this, &ASecurityCamera::OnTriggerOverlapBegin);
	coneShape->OnComponentEndOverlap.AddDynamic(this, &ASecurityCamera::OnTriggerOverlapEnd);

	if (pingAudioCue->IsValidLowLevelFast()) 
	{
		pingAudioComponent->SetSound(pingAudioCue);
	}

	CalculateBaseRotations();
}

// Called every frame
void ASecurityCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckUndetectedThieves();

	CheckAndRotate();

	if (frozen)
	{
		CheckUnfreeze(DeltaTime);
	}
}


void ASecurityCamera::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AThief>(OtherActor))
	{
		UndetectedThieves.Add(OtherActor);
	}
}

void ASecurityCamera::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("STOP SEARCHING!"));
	if(UndetectedThieves.Find(OtherActor) != INDEX_NONE)
		UndetectedThieves.Remove(OtherActor);
}

void ASecurityCamera::NotifyAllSecurity_Implementation(bool PingOrUnping)
{
	AGameGameMode* GameMode = (AGameGameMode*)GetWorld()->GetAuthGameMode();

	for (AActor* Officer : GameMode->TeamB)
	{
		if (PingOrUnping)
		{
			Cast<AOfficer>(Officer)->ReceiveCameraPing(cameraNumber);
		}
		else
			Cast<AOfficer>(Officer)->ReceiveCameraUnPing();
	}

	if (PingOrUnping)
	{
		hasPinged = true;
		PlayAudio();
	}
	else
		hasPinged = false;
		
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

	if (rotationDirection == 0) //Right
	{
		FRotator newRotation = GetActorForwardVector().RotateAngleAxis(0.3f, GetActorUpVector()).Rotation();
		SetActorRotation(newRotation);
		float Angle = -acos(FVector::DotProduct(referenceForwardVector, GetActorForwardVector())) * 180/PI;

		if (Angle < -rotateAmount)
		{
			rotationDirection = 1;
		}
	}
	else if (rotationDirection == 1) //left
	{
		FRotator newRotation = GetActorForwardVector().RotateAngleAxis(-0.3f, GetActorUpVector()).Rotation();
		SetActorRotation(newRotation);
		float Angle = acos(FVector::DotProduct(referenceForwardVector, GetActorForwardVector())) * 180 / PI;

		if (Angle > rotateAmount)
		{
			rotationDirection = 0;
		}
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
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SEARCHING!"));
			End = Thief->GetActorLocation();
			GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldDynamic, CollisionParams);

			if (Hit.GetActor() == Thief)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Found!"));
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

void ASecurityCamera::Ser_FreezeCamera_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Server CAMERA FROZEN!"));
	Mul_FreezeCamera();
}

void ASecurityCamera::Mul_FreezeCamera_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Multi CAMERA FROZEN!"));
	frozen = true;
	currentTimetoUnfreeze = timeToUnfreeze;
}


void ASecurityCamera::CheckUnfreeze(float Deltatime)
{
	if (currentTimetoUnfreeze <= 0)
	{
		Ser_UnFreezeCamera();
	}
	else
	{
		currentTimetoUnfreeze -= Deltatime;
	}
}


void ASecurityCamera::Ser_UnFreezeCamera_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SERVER CAMERA UNFREEZING!"));
	Mul_UnFreezeCamera();
}


void ASecurityCamera::Mul_UnFreezeCamera_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("MULTI CAMERA UNFREEZING!"));
	frozen = false;
}