// Fill out your copyright notice in the Description page of Project Settings.


#include "SensorGadget.h"
#include "Components/StaticMeshComponent.h"
#include "Base3C.h"
#include <Net/UnrealNetwork.h>
#include "Components/BoxComponent.h"
ASensorGadget::ASensorGadget()
{

	PrimaryActorTick.bCanEverTick = true;
	sensorGadgetMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh1"));
	sensorGadgetMesh1->SetupAttachment(RootComponent);
	sensorGadgetMesh1->SetIsReplicated(true);
	sensorGadgetMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh2"));
	sensorGadgetMesh2->SetIsReplicated(true);
	sensorGadgetMesh2->SetupAttachment(RootComponent);

	CollisionMesh = CreateDefaultSubobject<UBoxComponent>(FName("MiddleMesh"));
	CollisionMesh->SetupAttachment(RootComponent);
	CollisionMesh->SetIsReplicated(true);

	bReplicates = true;
}

void ASensorGadget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASensorGadget, placedActor); //Should replicate placedActor variable to all clients
}

// Called when the game starts or when spawned
void ASensorGadget::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ASensorGadget::OnOverlapBegin);
}

void ASensorGadget::OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ABase3C::StaticClass()) && placedActor != OtherActor && !pinged)
	{
		if (!placedActor)
			return;

		Ser_PingPlayer(OtherActor);
		Cast<AOfficer>(placedActor)->SetOfficerSensorScalor(1); //Need to set this for the material

		if (GetWorldTimerManager().IsTimerActive(RevealTimer))
			GetWorldTimerManager().ClearTimer(RevealTimer);

		GetWorldTimerManager().SetTimer(
			RevealTimer, this, &ASensorGadget::UnPingPlayer, revealTime, false);
	}
}

void ASensorGadget::SetRevealTime(float pRevealTime)
{
	revealTime = pRevealTime;
}

void ASensorGadget::CalculateMiddleMesh() //Computes middle mesh to fit in between Mesh1 and Mesh2
{
	FVector Location = (sensorGadgetMesh1->GetComponentLocation() + sensorGadgetMesh2->GetComponentLocation()) / 2;
	FRotator Rotation = sensorGadgetMesh1->GetUpVector().Rotation();

	float Scale = FVector::Distance(sensorGadgetMesh1->GetComponentLocation(), sensorGadgetMesh2->GetComponentLocation()) / 2;
	FTransform newTrans = FTransform(Rotation.Quaternion(), Location, FVector(Scale / 50, 0.1f, 0.1f));
	
	CollisionMesh->SetWorldTransform(newTrans);
}
void ASensorGadget::SetOfficerOwner(AActor* pOwner) //Set the player who placed the Trip
{
	if (!placedActor)
	{
		Ser_SetOfficer(pOwner);
	}

}

void ASensorGadget::Ser_PingPlayer_Implementation(AActor* pPlayerToPing)
{
	if (Cast<ABase3C>(pPlayerToPing)->GetMesh())
	{
		Cast<ABase3C>(pPlayerToPing)->ChangeStencilFromServer(2);
		pingedActor = pPlayerToPing;
	}
}

void ASensorGadget::Ser_UnPingPlayer_Implementation(AActor* pPlayerToPing)
{
	if (Cast<ABase3C>(pPlayerToPing)->GetMesh())
	{
		Cast<ABase3C>(pPlayerToPing)->ChangeStencilFromServer(0);
	}
}

void ASensorGadget::UnPingPlayer()
{
	Cast<AOfficer>(placedActor)->SetOfficerSensorScalor(0);
	Ser_UnPingPlayer(pingedActor);
}

void ASensorGadget::Ser_SetOfficer_Implementation(AActor* pOwner)
{
	placedActor = pOwner;
}