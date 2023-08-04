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

	UFUNCTION()
		void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION(Server, Reliable)
		void NotifyAllSecurity(class AThief* PingedActor);

	
};
