#include "SensorGadgetOfficerComponent.h"
#include "Materials/Material.h"
#include "SensorGadget.h"

USensorGadgetOfficerComponent::USensorGadgetOfficerComponent()
{
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

void USensorGadgetOfficerComponent::updatePosing(FVector CamLocation, FVector CamForward)
{
	bool state = ValidFirstPosition(CamLocation, CamForward);
	state = state && ValidSecondPosition(sensorGadgetOfficerMesh1->GetComponentLocation(), MeshNormal);

	ChangeMaterial(state);
}

void USensorGadgetOfficerComponent::ToggleEnable(bool Enabled)
{
	if (!sensorGadgetOfficerMesh1 || !sensorGadgetOfficerMesh2)
		return;

	sensorGadgetOfficerMesh1->SetVisibility(Enabled);
	sensorGadgetOfficerMesh2->SetVisibility(Enabled);
}

bool USensorGadgetOfficerComponent::ValidFirstPosition(FVector CamLocation, FVector CamForward)
{
	FHitResult Hit(ForceInit);
	FVector Start = CamLocation;
	FVector End = Start + CamForward * viewReach;
	FCollisionQueryParams CollisionParams;

	CollisionParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldDynamic, CollisionParams);

	if (Hit.IsValidBlockingHit())
	{
		if (Hit.GetComponent() == sensorGadgetOfficerMesh1 || Hit.GetActor()->IsA(ABase3C::StaticClass()))
			return false;

		if (Cast<ASensorGadget>(Hit.GetActor()) != nullptr)
			return false;

		ToggleEnable(true);

		MeshNormal = Hit.ImpactNormal;
		FRotator Rotation = FRotationMatrix::MakeFromX(MeshNormal).Rotator();
		FRotator Offset = FRotator(90, 180, 0);
		Rotation += Offset;

		FVector Location = Hit.Location + (MeshNormal * 5);

		sensorGadgetOfficerMesh1->SetWorldLocation(Location);
		sensorGadgetOfficerMesh1->SetWorldRotation(Rotation);

		firstLocation = Location;
		firstRotation = Rotation;

		return true;
	}

	ToggleEnable(false);
	CanPlace = false;

	return false;
}

bool USensorGadgetOfficerComponent::ValidSecondPosition(FVector FirstLocation, FVector ForwardVector)
{
	FHitResult Hit(ForceInit);
	FVector Start = FirstLocation;
	FVector End = Start + ForwardVector * range;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldDynamic, CollisionParams);

	if (Hit.IsValidBlockingHit())
	{
		if (Hit.GetComponent() == sensorGadgetOfficerMesh2 || Hit.GetActor()->IsA(ABase3C::StaticClass()))
			return  false;

		FVector Normal = Hit.ImpactNormal;
		FRotator Rotation = FRotationMatrix::MakeFromX(Normal).Rotator();
		FRotator Offset = FRotator(90, 180, 0);
		Rotation += Offset;

		FVector Location = Hit.Location + (Normal * 5);

		sensorGadgetOfficerMesh2->SetWorldLocation(Location);
		sensorGadgetOfficerMesh2->SetWorldRotation(Rotation);
		
		secondLocation = Location;
		secondRotation = Rotation;

		return true;
	}

	sensorGadgetOfficerMesh2->SetVisibility(false);
	return false;
}

void USensorGadgetOfficerComponent::ChangeMaterial(bool approved)
{
	if (approved)
	{
		sensorGadgetOfficerMesh1->SetMaterial(0, ApproveMaterial);
		sensorGadgetOfficerMesh2->SetMaterial(0, ApproveMaterial);
		CanPlace = true;
	}
	else
	{
		sensorGadgetOfficerMesh1->SetMaterial(0, RejectMaterial);
		sensorGadgetOfficerMesh2->SetMaterial(0, RejectMaterial);
		CanPlace = false;
	}
}

void USensorGadgetOfficerComponent::Place()
{
	if (!CanPlace || sensorsUsed >= maxSensors)
		return;

	sensorsUsed++;

	ServerSpawnSensor(firstLocation, firstRotation, secondLocation, secondRotation, Cast<AOfficer>(GetOwner()));

	CanPlace = false;
}

void USensorGadgetOfficerComponent::ServerSpawnSensor_Implementation(FVector pfirstLocation, FRotator pfirstRotation, FVector psecondLocation, FRotator psecondRotation, AOfficer* pOwner)
{
	FActorSpawnParameters SpawnInfo;
	ASensorGadget* Sensor = Cast<ASensorGadget>(GetWorld()->SpawnActor<AActor>(ActorTospawn, pfirstLocation, pfirstRotation, SpawnInfo));
	Sensor->sensorGadgetMesh1->SetWorldLocation(pfirstLocation);
	Sensor->sensorGadgetMesh1->SetWorldRotation(pfirstRotation);

	Sensor->sensorGadgetMesh2->SetWorldLocation(psecondLocation);
	Sensor->sensorGadgetMesh2->SetWorldRotation(psecondRotation);

	Sensor->CalculateMiddleMesh();
	Sensor->MULSetOfficer(pOwner);
	Sensor->SetRevealTime(revealTime);
}

bool USensorGadgetOfficerComponent::HasUnusedSensor() 
{ 
	return sensorsUsed < maxSensors; 
}