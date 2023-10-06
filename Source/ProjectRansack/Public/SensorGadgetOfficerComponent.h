// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon.h"
#include "SensorGadgetOfficerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API USensorGadgetOfficerComponent : public UActorComponent, public IWeapon
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

	void Tick_Implementation(float delta) override;

	void fetchData(float pRange, float pRevealTime, unsigned int pMaxSensors);

	void updatePosing(FVector CamLocation, FVector CamForward);

	bool ValidFirstPosition(FVector CamLocation, FVector CamForward);

	bool ValidSecondPosition(FVector FirstLocation, FVector ForwardVector);

	void ChangeMaterial(bool approved);

	UFUNCTION(Server, Reliable)
	void ServerSpawnSensor(FVector pfirstLocation, FRotator pfirstRotation, FVector psecondLocation, FRotator psecondRotation, class AOfficer* pOwner);
	void ServerSpawnSensor_Implementation(FVector pfirstLocation, FRotator pfirstRotation, FVector psecondLocation, FRotator psecondRotation, class AOfficer* pOwner);

	bool HasUnusedSensor();

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlToggleVisibility(bool visible);
	virtual void MUlToggleVisibility_Implementation(bool visible);

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlFire();
	virtual void MUlFire_Implementation();

	void UpdateUI_Implementation() override;
};
