// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SecurityCamera.generated.h"

UCLASS()
class PROJECTRANSACK_API ASecurityCamera : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASecurityCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* coneShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
		class USpotLightComponent* spotLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
		int cameraNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
		class USoundCue* pingAudioCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecurityCamera, meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* pingAudioComponent;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* cameraMesh = nullptr;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
		float rotateAmount = 30; //Amount to rotate in degrees

	int rotationDirection = 0; //0 Right 1 Left

	FVector referenceForwardVector = FVector();

	TArray<AActor*> UndetectedThieves;

	bool hasPinged = false;

	UFUNCTION()
		void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION(Server, Reliable)
		void NotifyAllSecurity(bool PingOrUnping);

	
	UFUNCTION(NetMulticast, Reliable)
		void PlayAudio();

	void CalculateBaseRotations();

	void CheckAndRotate();

	void CheckUndetectedThieves();
};
