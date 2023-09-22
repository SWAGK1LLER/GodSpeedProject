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


	entranceArea2 = CreateDefaultSubobject<UBoxComponent>(FName("entranceArea2"));
	entranceArea2->SetGenerateOverlapEvents(true);
	entranceArea2->SetupAttachment(ventArea);

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
	AThief* player = Cast<AThief>(OtherActor);
	if (player == nullptr)
		return;

	player->closeItems.Add(this);
}

void AventEntrance::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AThief* player = Cast<AThief>(OtherActor);
	if (player == nullptr)
		return;

	player->closeItems.Remove(this);
}

void AventEntrance::Interact_Implementation(AActor* pActor)
{
	if (!IsUsed)
	{
		if (pActor->IsA(AThief::StaticClass()))
		{
			float Dist1 = FVector::Dist(pActor->GetActorLocation(), entranceArea->GetComponentLocation());
			float Dist2 = FVector::Dist(pActor->GetActorLocation(), entranceArea2->GetComponentLocation());

			IsUsed = true;
			if (Dist1 < Dist2)
			{
				pActor->SetActorLocation(entranceArea2->GetComponentLocation());
			}
			else
			{
				pActor->SetActorLocation(entranceArea->GetComponentLocation());
			}
		}
	}
}

void AventEntrance::StopInteract_Implementation(AActor* pActor)
{
	if (pActor->IsA(AThief::StaticClass()))
	{
		IsUsed = false;
	}
}

