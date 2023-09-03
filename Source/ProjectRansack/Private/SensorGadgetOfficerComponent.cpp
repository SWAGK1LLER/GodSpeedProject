// Fill out your copyright notice in the Description page of Project Settings.


#include "SensorGadgetOfficerComponent.h"
#include "Materials/Material.h"
#include "SensorGadget.h"

// Sets default values for this component's properties
USensorGadgetOfficerComponent::USensorGadgetOfficerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	sensorGadgetOfficerMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorGadgetMesh1"));

	sensorGadgetOfficerMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorGadgetMesh2"));

	sensorGadgetOfficerMesh1->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	sensorGadgetOfficerMesh1->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);

	sensorGadgetOfficerMesh2->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	sensorGadgetOfficerMesh2->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
}

void USensorGadgetOfficerComponent::fetchData(float pRange, float pRevealTime, unsigned int pMaxSensors)
{
	range = pRange;
	revealTime = pRevealTime;
	maxSensors = pMaxSensors;
}

void USensorGadgetOfficerComponent::ToggleEnable(bool Enabled)
{
	if (!sensorGadgetOfficerMesh1 || !sensorGadgetOfficerMesh2)
		return;

	sensorGadgetOfficerMesh1->SetVisibility(Enabled);
	sensorGadgetOfficerMesh2->SetVisibility(Enabled);
}

void USensorGadgetOfficerComponent::CalculateFirstPosition(AActor* IgnoredSelf, FVector CamLocation, FVector CamForward)
{
	FHitResult Hit(ForceInit);
	FVector Start = CamLocation;
	FVector End = Start + CamForward * 1000;
	FCollisionQueryParams CollisionParams;

	CollisionParams.AddIgnoredActor(IgnoredSelf);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 2.f, false, 4.f);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldDynamic, CollisionParams);

	if (Hit.IsValidBlockingHit())
	{
		if (Hit.GetComponent() == sensorGadgetOfficerMesh1)
			return;

		if (Cast<ASensorGadget>(Hit.GetActor()) != nullptr)
		{
			ChangeMaterial(false);
			return;
		}

		ToggleEnable(true);
		FVector Point = Hit.ImpactPoint + (Hit.ImpactNormal * 0.01f);
		FVector Normal = Hit.ImpactNormal;

		FRotator Rotation = FRotationMatrix::MakeFromX(Normal).Rotator();
		FRotator Offset = FRotator(90, 180, 0);
		Rotation += Offset;
		sensorGadgetOfficerMesh1->SetWorldLocation(Point);
		sensorGadgetOfficerMesh1->SetWorldRotation(Rotation);

		firstLocation = Point;
		firstRotation = Rotation;

		CalculateSecondPosition(Point, Hit.ImpactNormal, IgnoredSelf);
	}
	else
	{
		ToggleEnable(false);
		CanPlace = false;
	}
}

void USensorGadgetOfficerComponent::CalculateSecondPosition(FVector FirstLocation, FVector ForwardVector, AActor* IgnoredSelf)
{
	FHitResult Hit(ForceInit);
	FVector Start = FirstLocation;
	FVector End = Start + ForwardVector * range;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(IgnoredSelf);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldDynamic, CollisionParams);

	if (Hit.IsValidBlockingHit())
	{
		if (Hit.GetComponent() == sensorGadgetOfficerMesh2 )//|| Hit.GetActor()->IsA(ABase3C::StaticClass()))
			return;

		FVector Point = Hit.ImpactPoint + (Hit.ImpactNormal * 0.01f);
		FVector Normal = Hit.ImpactNormal;

		FRotator Rotation = FRotationMatrix::MakeFromX(Normal).Rotator();
		FRotator Offset = FRotator(90, 180, 0);
		Rotation += Offset;
		sensorGadgetOfficerMesh2->SetWorldLocation(Point);
		sensorGadgetOfficerMesh2->SetWorldRotation(Rotation);
		ChangeMaterial(true);

		secondLocation = Point;
		secondRotation = Rotation;

		return;
		
	}

	sensorGadgetOfficerMesh2->SetVisibility(false);
	ChangeMaterial(false);
}

void USensorGadgetOfficerComponent::ChangeMaterial(bool approved)
{
	if (approved)
	{
		sensorGadgetOfficerMesh1->SetMaterial(0,ApproveMaterial);
		sensorGadgetOfficerMesh2->SetMaterial(0,ApproveMaterial);
		CanPlace = true;
	}
	else
	{
		sensorGadgetOfficerMesh1->SetMaterial(0, RejectMaterial);
		sensorGadgetOfficerMesh2->SetMaterial(0, RejectMaterial);
		CanPlace = false;
	}
}

void USensorGadgetOfficerComponent::TryPlace()
{
	if (!CanPlace || sensorsUsed >= maxSensors)
		return;

	

	FVector Location = (sensorGadgetOfficerMesh1->GetComponentLocation() + sensorGadgetOfficerMesh2->GetComponentLocation()) / 2;
	FRotator Rotation = sensorGadgetOfficerMesh1->GetComponentRotation();
	FRotator Offset = FRotator(90, 0, 0);
	Rotation += Offset;
	sensorsUsed++;

	ServerSpawnSensor(firstLocation, firstRotation, secondLocation, secondRotation, GetOwner());

	CanPlace = false;
}

void USensorGadgetOfficerComponent::ServerSpawnSensor_Implementation(FVector pfirstLocation, FRotator pfirstRotation, FVector psecondLocation, FRotator psecondRotation, AActor* pOwner)
{
	FActorSpawnParameters SpawnInfo;
	ASensorGadget* Sensor = Cast<ASensorGadget>(GetWorld()->SpawnActor<AActor>(ActorTospawn, pfirstLocation, pfirstRotation, SpawnInfo));
	Sensor->sensorGadgetMesh1->SetWorldLocation(pfirstLocation);
	Sensor->sensorGadgetMesh1->SetWorldRotation(pfirstRotation);

	Sensor->sensorGadgetMesh2->SetWorldLocation(psecondLocation);
	Sensor->sensorGadgetMesh2->SetWorldRotation(psecondRotation);

	Sensor->CalculateMiddleMesh();
	Sensor->SetOfficerOwner(pOwner);
	Sensor->SetRevealTime(revealTime);
}
