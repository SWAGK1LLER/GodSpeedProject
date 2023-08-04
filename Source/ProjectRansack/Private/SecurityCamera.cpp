// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Officer.h"
#include "Thief.h"
#include "GameGameMode.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

ASecurityCamera::ASecurityCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	spotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	spotLight->SetupAttachment(RootComponent);

	coneShape = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cone"));
	coneShape->SetupAttachment(spotLight);


	pingAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CameraPingAudio"));
	pingAudioComponent->bAutoActivate = false;
	pingAudioComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASecurityCamera::BeginPlay()
{
	Super::BeginPlay();

	coneShape->OnComponentBeginOverlap.AddDynamic(this, &ASecurityCamera::OnTriggerOverlapBegin);

	if (pingAudioCue->IsValidLowLevelFast()) {
		pingAudioComponent->SetSound(pingAudioCue);
	}
	
}

// Called every frame
void ASecurityCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASecurityCamera::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Seeing player!"));

	if (Cast<AThief>(OtherActor))
	{
		NotifyAllSecurity(Cast<AThief>(OtherActor));
	}
}

void ASecurityCamera::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


void ASecurityCamera::NotifyAllSecurity_Implementation(AThief* PingedActor)
{
	AGameGameMode* GameMode = (AGameGameMode*)GetWorld()->GetAuthGameMode();

	for (AActor* Officer : GameMode->TeamB)
	{
		Cast<AOfficer>(Officer)->ReceiveCameraPing(cameraNumber);
	}

	PlayAudio();
}


void ASecurityCamera::PlayAudio_Implementation() //this probably works but I can't test the sound from one computer
{
	pingAudioComponent->Play();
}
