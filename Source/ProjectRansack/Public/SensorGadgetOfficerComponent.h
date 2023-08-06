// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SensorGadgetOfficerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API USensorGadgetOfficerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USensorGadgetOfficerComponent();

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* sensorGadgetOfficerMesh1;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* sensorGadgetOfficerMesh2;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Color, meta = (AllowPrivateAccess = "true"))
		class UMaterial* RejectMaterial;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Color, meta = (AllowPrivateAccess = "true"))
		class UMaterial* ApproveMaterial;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> ActorTospawn;

	void fetchData(float pRange, float pRevealTime, unsigned int pMaxSensors);

	void ToggleEnable(bool Enabled);

	void CalculateFirstPosition(AActor* IgnoredSelf, FVector CamLocation, FVector CamForward);

	void CalculateSecondPosition(FVector FirstLocation, FVector ForwardVector, AActor* IgnoredSelf);

	void ChangeMaterial(bool approved);

	void TryPlace();

	UFUNCTION(Server, Reliable)//ok so apparently if only the server spawns actors they will automatically replicate
		void ServerSpawnSensor(FVector pfirstLocation, FRotator pfirstRotation, FVector psecondLocation, FRotator psecondRotation, AActor* pOwner);

	class ASensorGadget* SpawnedSensor;

	float range = 0;

	float revealTime = 0;

	bool CanPlace = false;

	FVector firstLocation;
	FRotator firstRotation;

	FVector secondLocation;
	FRotator secondRotation;

	int maxSensors = 0;

	int sensorsUsed = 0;
};
