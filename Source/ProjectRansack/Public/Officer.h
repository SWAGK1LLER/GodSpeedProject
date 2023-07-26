// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base3C.h"
#include "OfficerInteractibleActor.h"
#include "Components/TimelineComponent.h"
#include "Officer.generated.h"

/**
 * 
 */
class UCurveFloat;

USTRUCT(BlueprintType)
struct FOfficerTable : public FTableRowBase
{
GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MotionVisionAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
		UCurveFloat* MotionVisionFloatCurve = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
		class UMaterialParameterCollection* MotionVisionMPC = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
		int MotionSensorStencilBufNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
		class UInputAction* FlashlightAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SensorGadgetAction = nullptr;
};

UCLASS()
class PROJECTRANSACK_API AOfficer : public ABase3C
{
	GENERATED_BODY()

public:
	TArray<IOfficerInteractibleActor*> closeItems;
	IOfficerInteractibleActor* ItemUsing = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
		class UDataTable* officerDataTable = nullptr;

	FOfficerTable* officerTableInstance = nullptr;

	FTimeline MotionVisionTimeline;

	bool MotionTimelineRunning = false;

	bool motionSensorActive = false;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Light, meta = (AllowPrivateAccess = "true"))
		class USpotLightComponent* flashLight;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"), replicated)
		class USensorGadgetOfficerComponent* sensorGadgetOfficer;

	bool flashLightOn = false;

	bool usingSensorGadget = false;

	// Called to bind functionality to input
	AOfficer();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void CreateTimeline();

	UFUNCTION()
	void TimelineProgress(float value);

	UFUNCTION()
	void TimelineFinished();

	void ChangeStencilOnMovement();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void MulReset_Implementation() override;

	bool CheckTableInstance();

	void HandleMotionVision();

	void ToggleFlashight();

	void SensorGadgetAction();

	virtual void StartFire() override;

	virtual void Interact() override;
	virtual void StopInteract() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ArrestThief(ABase3C* other);
	void ArrestThief_Implementation(ABase3C* other);
};
