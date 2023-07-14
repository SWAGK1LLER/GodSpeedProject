#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "ThiefInteractibleActor.h"
#include "ItemWidgetUI.h"
#include "Item.generated.h"

USTRUCT(BlueprintType)
struct FSize
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SizeX = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SizeY = 1;
};

UCLASS()
class PROJECTRANSACK_API AItem : public AActor, public IThiefInteractibleActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSize SizeNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToPickUp = 0;
	float currentTime = 0;
	bool currentlyInteracting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UItemWidgetUI> WidgetClass;
	UItemWidgetUI* Widget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* Trigger = nullptr;

	class AActor* acteurUsingThis = nullptr;

	AItem();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	virtual void Interact_Implementation(class AActor* pActor) override;
	virtual void StopInteract_Implementation(class AActor* pActor) override;

	float map(float x, float in_min, float in_max, float out_min, float out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}
};
