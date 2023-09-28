#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include <MagneticDoorUI.h>
#include "MagneticDoor.generated.h"

UCLASS()
class PROJECTRANSACK_API AMagneticDoor : public ADoor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* secondDoor = nullptr;

	FVector startingLocationDoor2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMagneticDoorUI> WidgetClass;
	UMagneticDoorUI* Widget = nullptr;

	class AActor* acteurUsingThis = nullptr;

	float currentTime = 0;
	bool currentlyInteracting = false;
	bool manullyOpen = false;

	float time = 0;

	AMagneticDoor();

	virtual void BeginPlay() override;

	void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void StopInteract_Implementation(class AActor* pActor) override;

	void StartInteract(class AActor* pActor, bool pManullyOpen = false) override;

	void UpdateUIText_Implementation() override;

	void TimelineProgress(float value) override;
};
