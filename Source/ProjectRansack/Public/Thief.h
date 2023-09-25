// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base3C.h"
#include "Inventory.h"
#include "ThiefInteractibleActor.h"
#include "EOSGameInstance.h"
#include "ArrestUI.h"
#include <Officer.h>
#include "Thief.generated.h"

USTRUCT(BlueprintType)
struct FThiefTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
	class UInputAction* NightVisionAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* NightVisionFloatCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
	class UMaterialParameterCollection* NightVisionMPC = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int costOnArrest = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int respawnTime = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* crouchAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* climbAction = nullptr;
};

UCLASS()
class PROJECTRANSACK_API AThief : public ABase3C
{
	GENERATED_BODY()

public:
	TArray<IThiefInteractibleActor*> closeItems;
	IThiefInteractibleActor* ItemUsing = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	class UDataTable* thiefDataTable = nullptr;

	FThiefTable* thiefTableInstance = nullptr;

	FTimeline NightVisionTimeline;

	bool NightTimelineRunning = false;
	bool NightSensorActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* ArrestArea = nullptr;
	bool ArrestAreaActivate = false;

	bool beingArrest = false;
	AOfficer* officerArresting = nullptr;
	TArray<ABase3C*> closeOfficer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInventory* inventory = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UArrestUI> ArrestWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UArrestUI> ArrestOfficerWidgetClass;

	UArrestUI* ArrestUISelf = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* ClimbingArea = nullptr;

	bool CanClimb = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool IsClimbing = false;

	AThief();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetClientUI_Implementation() override;

	bool CreateTableInstance() override;

	void CreateTimeline();

	UFUNCTION()
	void TimelineProgress2(float value);

	UFUNCTION()
	void TimelineFinished2();

	virtual void Move(const FInputActionValue& Value) override;

	virtual void SRReset_Implementation() override;
	virtual void MulReset_Implementation(FTransform transform) override;

	void HandleNightVision();

	bool HasSpaceForItem(class AItem* pItem);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRAddItem(class AItem* pItem);
	void SRAddItem_Implementation(class AItem* pItem);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlAddItem(class AItem* pItem);
	void MUlAddItem_Implementation(class AItem* pItem);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRClearItems(int score, ETeam pTeam);
	void SRClearItems_Implementation(int score, ETeam pTeam);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlClearItems(int score);
	void MUlClearItems_Implementation(int score);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRDropInventory(FVector location);
	void SRDropInventory_Implementation(FVector location);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlDropInventory(FVector location);
	void MUlDropInventory_Implementation(FVector location);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRActivateArrestTrigger();
	void SRActivateArrestTrigger_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulActivateArrestTrigger();
	void MulActivateArrestTrigger_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulSetBeingArrest(bool pArrest, AOfficer* pOfficer);
	void MulSetBeingArrest_Implementation(bool pArrest, AOfficer* pOfficer);

	void ResetOfficerInArrestArea(AOfficer* pOfficerToSkip = nullptr);

	void CheckCanClimb();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRStartClimbing();
	void SRStartClimbing_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulStartClimbing();
	void MulStartClimbing_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SRStopClimbing();
	void SRStopClimbing_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulStopClimbing();
	void MulStopClimbing_Implementation();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientShowArrest(bool pArrest);
	void ClientShowArrest_Implementation(bool pArrest);

	bool ValidateSpaceItem(class AItem& pItem);

	void ChangeStencilOnMovement();

	virtual void Interact() override;
	virtual void StopInteract() override;

	virtual void Tab() override;
	virtual void StopTab() override;

	void ClientFreezeInput_Implementation(float duration) override;

	virtual void UnFreezeInput_Implementation() override;

	void crouch();
	void unCrouch();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlCopyMagnetCard();
	void MUlCopyMagnetCard_Implementation();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void CopyMagnetCardUI(bool state);
	void CopyMagnetCardUI_Implementation(bool state);

	UFUNCTION()
	void OnArrestTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnArrestTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ClimbTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ClimbTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FHitResult ClimbingLineTrace();
};
