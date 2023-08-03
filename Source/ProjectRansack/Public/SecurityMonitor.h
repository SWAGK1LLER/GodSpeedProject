// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OfficerInteractibleActor.h"
#include "SecurityMonitor.generated.h"

UCLASS()
class PROJECTRANSACK_API ASecurityMonitor : public AActor, public IOfficerInteractibleActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASecurityMonitor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* monitorMesh;

	TArray<AActor*>Cameras;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UShapeComponent* Trigger = nullptr;

	AActor* currentCamera;

	int currentCameraIndex = 0;

	int maxCameraIndex = 0;

	bool IsInCamera = false;

	APlayerController* Controller = nullptr;


	UFUNCTION()
		void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	virtual void Interact_Implementation(class AActor* pActor) override;
	virtual void StopInteract_Implementation(class AActor* pActor) override;

	
	void SwitchCameraRight();

	void SwitchCameraLeft();
};
