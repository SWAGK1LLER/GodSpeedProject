// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThiefInteractibleActor.h"
#include "ItemWidgetUI.h"
#include "LightFuseBoxe.generated.h"

UCLASS()
class PROJECTRANSACK_API ALightFuseBoxe : public AActor, public IThiefInteractibleActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* Trigger = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UItemWidgetUI> WidgetClass;
	UItemWidgetUI* Widget = nullptr;

	class AActor* acteurUsingThis = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ALight*> ConnectedLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToInteract = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LightOffDuration = 2;

	float currentTime = 0;
	bool currentlyInteracting = false;

	bool FuseStateOpen = true;
	bool FuseBoxHacked = false;

	ALightFuseBoxe();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void Interact_Implementation(class AActor* pActor) override;
	virtual void StopInteract_Implementation(class AActor* pActor) override;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void HackLights();
	void HackLights_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void ToggleLights(bool pOpen);
	void ToggleLights_Implementation(bool pOpen);
};
