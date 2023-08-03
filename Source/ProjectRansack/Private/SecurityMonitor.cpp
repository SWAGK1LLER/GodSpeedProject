// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityMonitor.h"
#include "Components/StaticMeshComponent.h"
#include "SecurityCamera.h"
#include <Kismet/GameplayStatics.h>
#include <Components/BoxComponent.h>
#include "Officer.h"
#include "CameraComp.h"
//Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
///////////////
// Sets default values
ASecurityMonitor::ASecurityMonitor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	monitorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	monitorMesh->SetupAttachment(RootComponent);

	Trigger = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	Trigger->SetupAttachment(monitorMesh);
	Trigger->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ASecurityMonitor::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASecurityCamera::StaticClass(), Cameras);

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ASecurityMonitor::OnTriggerOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ASecurityMonitor::OnTriggerOverlapEnd);

	currentCamera = Cameras[0];

	maxCameraIndex = Cameras.Num();
}

void ASecurityMonitor::SwitchCameraRight()
{
	if (!IsInCamera || !Controller)
		return;

	FViewTargetTransitionParams Params;
	if (currentCameraIndex < maxCameraIndex - 1)
	{
		currentCameraIndex++;
		currentCamera = Cameras[currentCameraIndex];
		Controller->SetViewTarget(currentCamera, Params);
	}
	else
	{
		currentCameraIndex = 0;
		currentCamera = Cameras[currentCameraIndex];
		Controller->SetViewTarget(currentCamera, Params);
	}
}

void ASecurityMonitor::SwitchCameraLeft()
{
	if (!IsInCamera || !Controller)
		return;

	FViewTargetTransitionParams Params;

	if (currentCameraIndex != 0)
	{
		currentCameraIndex--;
		currentCamera = Cameras[currentCameraIndex];
		Controller->SetViewTarget(currentCamera, Params);
	}
	else
	{
		currentCameraIndex = maxCameraIndex - 1;
		currentCamera = Cameras[currentCameraIndex];
		Controller->SetViewTarget(currentCamera, Params);
	}
}

// Called every frame
void ASecurityMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASecurityMonitor::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AOfficer* player = Cast<AOfficer>(OtherActor);
	if (player == nullptr)
		return;

	player->closeItems.Add(this);
}

void ASecurityMonitor::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AOfficer* player = Cast<AOfficer>(OtherActor);
	if (player == nullptr)
		return;

	player->closeItems.Remove(this);
}

void ASecurityMonitor::Interact_Implementation(AActor* pActor)
{


	if (!IsInCamera)
	{
		FViewTargetTransitionParams Params;
		APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(pActor, 0);

		if (!Controller)
			Controller = OurPlayerController;


		Cast<ABase3C>(pActor)->ClientFreezeInput(INFINITY);

		OurPlayerController->SetViewTarget(currentCamera, Params);


		IsInCamera = true;
	}
	else
	{
		FViewTargetTransitionParams Params;
		APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(pActor, 0);

		OurPlayerController->SetViewTarget(Cast<ABase3C>(pActor), Params);

		Cast<ABase3C>(pActor)->UnFreezeInput();

		IsInCamera = false;
	}
}

void ASecurityMonitor::StopInteract_Implementation(AActor* pActor)
{
}


