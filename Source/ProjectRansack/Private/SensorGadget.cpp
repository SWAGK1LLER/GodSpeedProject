// Fill out your copyright notice in the Description page of Project Settings.


#include "SensorGadget.h"
#include "Components/StaticMeshComponent.h"
#include "Base3C.h"
#include <Net/UnrealNetwork.h>
// Sets default values
ASensorGadget::ASensorGadget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	sensorGadgetMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh1"));
	sensorGadgetMesh1->SetupAttachment(RootComponent);
	sensorGadgetMesh1->SetIsReplicated(true);
	sensorGadgetMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh2"));
	sensorGadgetMesh2->SetIsReplicated(true);
	sensorGadgetMesh2->SetupAttachment(RootComponent);

	MiddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("MiddleMesh"));
	MiddleMesh->SetupAttachment(RootComponent);
	MiddleMesh->SetIsReplicated(true);

	bReplicates = true;
	MiddleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ASensorGadget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASensorGadget, placedActor);
}

void ASensorGadget::OverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ABase3C::StaticClass()) && placedActor != OtherActor && !pinged)
	{
		Ser_PingPlayer(OtherActor);
		Cast<AOfficer>(placedActor)->SetOfficerSensorScalor(1);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ACTOR HIT NAME %s"), *OtherActor->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OWNER ACTOR NAME %s"), *placedActor->GetName()));
	//	Cast<AOfficer>(placedActor)->TimelineProgress(1);
	}
}

void ASensorGadget::Ser_PingPlayer_Implementation(AActor* pPlayerToPing)
{
	if (Cast<ABase3C>(pPlayerToPing)->GetMesh())
	{
		
		//Cast<ABase3C>(pPlayerToPing)->Revealed = true;
		Cast<AOfficer>(pPlayerToPing)->ChangeStencilFromServer(2);
		
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ACTOR PINGED NAME %s"), *pPlayerToPing->GetName()));
		//pinged = true;

	}
}

// Called when the game starts or when spawned
void ASensorGadget::BeginPlay()
{
	MiddleMesh->OnComponentBeginOverlap.AddDynamic(this, &ASensorGadget::OverlapBegin);
	Super::BeginPlay();
}

void ASensorGadget::CalculateMiddleMesh()
{
	FVector Location = (sensorGadgetMesh1->GetComponentLocation() + sensorGadgetMesh2->GetComponentLocation()) / 2;
	FRotator Rotation = sensorGadgetMesh1->GetUpVector().Rotation();

	float Scale = FVector::Distance(sensorGadgetMesh1->GetComponentLocation(), sensorGadgetMesh2->GetComponentLocation()) / 2;
	FTransform newTrans = FTransform(Rotation.Quaternion(), Location, FVector(Scale / 50, 0.1f, 0.1f));
	MiddleMesh->SetWorldTransform(newTrans);

}
void ASensorGadget::SetOfficerOwner(AActor* pOwner)
{
	if (!placedActor)
	{
		Ser_SetOfficer(pOwner);
	}

}

void ASensorGadget::Ser_SetOfficer_Implementation(AActor* pOwner)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ACTOR NAME %s"), *pOwner->GetName()));
	placedActor = pOwner;
	//Mul_SetOfficer(pOwner);
}


// Called every frame
void ASensorGadget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

