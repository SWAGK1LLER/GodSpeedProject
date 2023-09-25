#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "SlidingDoorUI.h"
#include "SlidingDoor.generated.h"

UCLASS()
class PROJECTRANSACK_API ASlidingDoor : public ADoor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USlidingDoorUI> WidgetClass;
	USlidingDoorUI* Widget = nullptr;

	class AActor* acteurUsingThis = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToHackThief = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToFixOfficer = 2;

	float currentTime = 0;
	bool currentlyInteracting = false;
	bool manullyOpen = false;

	bool FuseBoxHacked = false;

	float time = 0;

	ASlidingDoor();

	virtual void Tick(float DeltaTime) override;

	void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void StopInteract_Implementation(class AActor* pActor) override;

	void StartInteract(class AActor* pActor, bool pManullyOpen = false) override;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void ToogleHackDoor(bool isHack);
	void ToogleHackDoor_Implementation(bool isHack);

	void UpdateProgressHack(float DeltaTime);

	void UpdateUIText_Implementation() override;
};
