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

	class ASensorGadget* SpawnedSensor;

	float range = 0;

	float viewReach = 1000;

	float revealTime = 0;

	bool CanPlace = false;

	FVector firstLocation;
	FRotator firstRotation;

	FVector MeshNormal;

	FVector secondLocation;
	FRotator secondRotation;

	int maxSensors = 2;

	int sensorsUsed = 0;

	USensorGadgetOfficerComponent();

	void fetchData(float pRange, float pRevealTime, unsigned int pMaxSensors);

	void updatePosing(FVector CamLocation, FVector CamForward);
	
	void ToggleEnable(bool Enabled);

	bool ValidFirstPosition(FVector CamLocation, FVector CamForward);

	bool ValidSecondPosition(FVector FirstLocation, FVector ForwardVector);

	void ChangeMaterial(bool approved);

	void Place();

	UFUNCTION(Server, Reliable)
	void ServerSpawnSensor(FVector pfirstLocation, FRotator pfirstRotation, FVector psecondLocation, FRotator psecondRotation, class AOfficer* pOwner);
	void ServerSpawnSensor_Implementation(FVector pfirstLocation, FRotator pfirstRotation, FVector psecondLocation, FRotator psecondRotation, class AOfficer* pOwner);

	bool HasUnusedSensor();
};
