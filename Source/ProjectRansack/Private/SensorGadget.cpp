#include "SensorGadget.h"
#include "Components/StaticMeshComponent.h"
#include "Thief.h"
#include "Components/BoxComponent.h"
#include <GamePlayerController.h>

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

void ASensorGadget::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ASensorGadget::OnOverlapBegin);
}

void ASensorGadget::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AThief* thief = Cast<AThief>(OtherActor);
	if (thief == nullptr)
		return;

	Cast<AGamePlayerController>(thief->GetController())->SendPingRequest(this, thief);
}

void ASensorGadget::MULSetOfficer_Implementation(AOfficer* pOwner)
{
	placedActor = pOwner;
}

void ASensorGadget::SetRevealTime(float pRevealTime)
{
	revealTime = pRevealTime;
}

void ASensorGadget::PingPlayer(AThief* pPlayerToPing, AOfficer* pOfficer)
{
	if (pPlayerToPing != nullptr)
	{
		if (spottedPlayer.Contains(pPlayerToPing))
			return;

		pOfficer->SetOfficerSensorScalor(1);

		pPlayerToPing->ChangeStencilFromServer(2);
		spottedPlayer.Add(pPlayerToPing);

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this, pPlayerToPing, pOfficer] {
			pPlayerToPing->ChangeStencilFromServer(0);
			spottedPlayer.Remove(pPlayerToPing);

			if (spottedPlayer.Num() == 0)
				pOfficer->SetOfficerSensorScalor(0);

		}), revealTime, false);
	}
}

void ASensorGadget::CalculateMiddleMesh() //Computes middle mesh to fit in between Mesh1 and Mesh2
{
	FVector Location = (sensorGadgetMesh1->GetComponentLocation() + sensorGadgetMesh2->GetComponentLocation()) / 2;
	FRotator Rotation = sensorGadgetMesh1->GetUpVector().Rotation();

	float Scale = FVector::Distance(sensorGadgetMesh1->GetComponentLocation(), sensorGadgetMesh2->GetComponentLocation()) / 2;
	FTransform newTrans = FTransform(Rotation.Quaternion(), Location, FVector(Scale, 1, 1));
	
	CollisionMesh->SetWorldTransform(newTrans);
}