#pragma once

#include "CoreMinimal.h"
#include "Base3C.h"
#include "OfficerInteractibleActor.h"
#include "Components/TimelineComponent.h"
#include "OfficerNotificationUI.h"
#include "OfficerInteractibleActor.h"
#include <StunOfficerUI.h>
#include "Officer.generated.h"

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

	//CAMERAS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RightAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LeftAction = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
	float Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
	float RevealTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
	int MaxAmountOfSensors;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TimeToArrestThief = 6;

	float arrestTime = 0;
	bool startArrest = false;
	ABase3C* ArrestingThief = nullptr;
	TArray<ABase3C*> closeThief;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UStunOfficerUI> StunOfficerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* StunArea = nullptr;
	bool StunAreaActivate = false;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = Light, meta = (AllowPrivateAccess = "true"))
	class USpotLightComponent* flashLight;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"), replicated)
	class USensorGadgetOfficerComponent* sensorGadgetOfficer;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UOfficerNotificationUI> notificationWidgetClass;

	UOfficerNotificationUI* notificationUI = nullptr;

	bool flashLightOn = false;

	bool usingSensorGadget = false;

	AOfficer();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool CreateTableInstance() override;
	
	void SendDataToComponents();

	void CreateTimeline();

	UFUNCTION()
	void TimelineProgress(float value);

	UFUNCTION()
	void TimelineFinished();

	virtual void MulReset_Implementation(FTransform transform) override;

	void ChangeStencilOnMovement();

	UFUNCTION(Client, Reliable)
	void SetOfficerSensorScalor(int newValue);

	void HandleMotionVision();

	void ToggleFlashight();

	void SensorGadgetAction();

	virtual void StartFire() override;

	virtual void Interact() override;
	virtual void StopInteract() override;

	void ClientFreezeInput_Implementation(float duration, AActor* pActor) override;
	virtual void UnFreezeInput_Implementation() override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRActivateArrestTrigger();
	void SRActivateArrestTrigger_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulActivateArrestTrigger();
	void MulActivateArrestTrigger_Implementation();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ArrestThief(ABase3C* other);
	void ArrestThief_Implementation(ABase3C* other);


	UFUNCTION(Client, Reliable)
	void ReceiveCameraPing(int CameraNumb);

	UFUNCTION(Client, Reliable)
	void ReceiveCameraUnPing();

	void SetupNotificationUI();

	UFUNCTION()
	void OnStunTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnStunTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
