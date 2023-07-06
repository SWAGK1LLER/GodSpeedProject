#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
class PROJECTRANSACK_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSize SizeNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToPickUp = 0;

	UPROPERTY(VisibleAnywhere)
	class UShapeComponent* Trigger = nullptr;

	AItem();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
