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
	CheckDataTable();

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
}


void ASecurityCamera::CheckDataTable()
{
	if (dataTable)
	{
		tableInstance = dataTable->FindRow<FSecurityCameraDataTable>(FName(TEXT("SecurityCamera")), "");
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


	if (GameMode->TeamB.Num() > 0)
	{
		if (GameMode->TeamB[0]->IsA(AOfficer::StaticClass()))
		{
			for (AActor* Officer : GameMode->TeamB)
			{
				if (PingOrUnping)
				{
					Cast<AOfficer>(Officer)->ReceiveCameraPing(cameraNumber);
				}
				else
					Cast<AOfficer>(Officer)->ReceiveCameraUnPing();
			}
		}
	}
	else if (GameMode->TeamA.Num() > 0)
	{
		if (GameMode->TeamA[0]->IsA(AOfficer::StaticClass()))
		{
			for (AActor* Officer : GameMode->TeamA)
			{
				if (PingOrUnping)
				{
					Cast<AOfficer>(Officer)->ReceiveCameraPing(cameraNumber);
				}
				else
					Cast<AOfficer>(Officer)->ReceiveCameraUnPing();
			}
		}
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

	if (rotationDirection == 0) //Right
	{
		FRotator newRotation = GetActorForwardVector().RotateAngleAxis(tableInstance->rotationSpeed, GetActorUpVector()).Rotation();
		SetActorRotation(newRotation);
		float Angle = -acos(FVector::DotProduct(referenceForwardVector, GetActorForwardVector())) * 180/PI;

		if (Angle < -tableInstance->rotateAmount)
		{
			rotationDirection = 1;
		}
	}
	else if (rotationDirection == 1) //left
	{
		FRotator newRotation = GetActorForwardVector().RotateAngleAxis(-tableInstance->rotationSpeed, GetActorUpVector()).Rotation();
		SetActorRotation(newRotation);
		float Angle = acos(FVector::DotProduct(referenceForwardVector, GetActorForwardVector())) * 180 / PI;

		if (Angle > tableInstance->rotateAmount)
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

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Multi CAMERA FROZEN!"));
	frozen = true;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CAMERA UNFREEZING!"));
		frozen = false;
	}), tableInstance->timeToUnfreeze, false);
}