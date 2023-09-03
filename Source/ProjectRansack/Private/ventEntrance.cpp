// Fill out your copyright notice in the Description page of Project Settings.


#include "ventEntrance.h"
#include <Components/BoxComponent.h>
#include "Thief.h"

// Sets default values
AventEntrance::AventEntrance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ventArea = CreateDefaultSubobject<UBoxComponent>(FName("ventArea"));
	ventArea->SetGenerateOverlapEvents(true);
	ventArea->SetupAttachment(RootComponent);

	entranceArea = CreateDefaultSubobject<UBoxComponent>(FName("entranceArea"));
	entranceArea->SetGenerateOverlapEvents(true);
	entranceArea->SetupAttachment(ventArea);

}

// Called when the game starts or when spawned
void AventEntrance::BeginPlay()
{
	Super::BeginPlay();

	ventArea->OnComponentBeginOverlap.AddDynamic(this, &AventEntrance::TriggerOverlapBegin);
	
}

// Called every frame
void AventEntrance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AventEntrance::TriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AThief::StaticClass()))
	{
		OtherActor->SetActorLocation(entranceArea->GetComponentLocation());
	}
}

