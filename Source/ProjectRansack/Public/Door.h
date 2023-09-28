#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThiefInteractibleActor.h"
#include "OfficerInteractibleActor.h"
#include "Components/TimelineComponent.h"
#include "Door.generated.h"

UENUM(BlueprintType)
enum class ESlidingAxis : uint8
{
	X,
	Y,
	Z
};

UCLASS()
class PROJECTRANSACK_API ADoor : public AActor, public IThiefInteractibleActor, public IOfficerInteractibleActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* Trigger = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
	ESlidingAxis slidingAxis = ESlidingAxis::X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionVision, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* doorAnimation = nullptr;

	FTimeline doorTimeLine;
	FVector startingLocation;

	bool animationRunning = false;

	class AActor* acteurUsingThis = nullptr;

	float currentTime = 0;
	bool currentlyInteracting = false;
	bool manullyOpen = false;

	bool FuseStateOpen = true;

	float time = 0;

	ADoor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void Interact_Implementation(class AActor* pActor) override;

	virtual void StartInteract(class AActor* pActor, bool pManullyOpen = false);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void ToggleDoor(bool pOpen, bool manully);
	void ToggleDoor_Implementation(bool pOpen, bool manully);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	virtual void UpdateUIText();
	virtual void UpdateUIText_Implementation();

	UFUNCTION()
	void TimelineProgress(float value);

	UFUNCTION()
	void TimelineFinished();

	void HandleDoorAnimation();
};
