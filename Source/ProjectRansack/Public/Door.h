// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThiefInteractibleActor.h"
#include "OfficerInteractibleActor.h"
#include <DoorUI.h>
#include "Door.generated.h"


UCLASS()
class PROJECTRANSACK_API ADoor : public AActor, public IThiefInteractibleActor, public IOfficerInteractibleActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* Trigger = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDoorUI> WidgetClass;
	UDoorUI* Widget = nullptr;

	class AActor* acteurUsingThis = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToInteract = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HackDuration = 2;

	float currentTime = 0;
	bool currentlyInteracting = false;

	bool FuseStateOpen = true;
	bool FuseBoxHacked = false;

	float time = 0;

	ADoor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void Interact_Implementation(class AActor* pActor) override;
	virtual void StopInteract_Implementation(class AActor* pActor) override;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void HackDoor();
	void HackDoor_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void ToggleDoor(bool pOpen);
	void ToggleDoor_Implementation(bool pOpen);

	void UpdateProgressHack(float DeltaTime);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void UpdateUIText();
	void UpdateUIText_Implementation();

};
