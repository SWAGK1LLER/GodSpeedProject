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

	void ToggleEnable(bool Enabled);

	UFUNCTION(NetMultiCast, Reliable) //TODO Fix the multiplayer
	void CalculateFirstPosition(AActor* IgnoredSelf, FVector CamLocation, FVector CamForward);

	UFUNCTION(NetMultiCast, Reliable)
	void CalculateSecondPosition(FVector FirstLocation, FVector ForwardVector, AActor* IgnoredSelf);

	void ChangeMaterial(bool approved);

	UFUNCTION(NetMultiCast, Reliable)
	void TryPlace();

	int Range = 1000;

	bool CanPlace = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
