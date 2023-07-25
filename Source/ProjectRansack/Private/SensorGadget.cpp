// Fill out your copyright notice in the Description page of Project Settings.


#include "SensorGadget.h"
#include "Components/StaticMeshComponent.h"
// Sets default values
ASensorGadget::ASensorGadget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	sensorGadgetMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh1"));
	sensorGadgetMesh1->SetupAttachment(RootComponent);
	sensorGadgetMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh2"));
	sensorGadgetMesh2->SetupAttachment(RootComponent);

	MiddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("MiddleMesh"));
	MiddleMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASensorGadget::BeginPlay()
{
	Super::BeginPlay();
}

void ASensorGadget::CalculateMiddleMesh_Implementation()
{
	FVector Location = (sensorGadgetMesh1->GetComponentLocation() + sensorGadgetMesh2->GetComponentLocation()) / 2;
	FRotator Rotation = sensorGadgetMesh1->GetUpVector().Rotation();

	float Scale = FVector::Distance(sensorGadgetMesh1->GetComponentLocation(), sensorGadgetMesh2->GetComponentLocation()) / 2;
	FTransform newTrans = FTransform(Rotation.Quaternion(), Location, FVector(Scale / 50, 0.1f, 0.1f));
	MiddleMesh->SetWorldTransform(newTrans);

}

// Called every frame
void ASensorGadget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

