#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThiefInteractibleActor.h"
#include "ventEntrance.generated.h"

UCLASS()
class PROJECTRANSACK_API AventEntrance : public AActor, public IThiefInteractibleActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* ventArea = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* entranceArea = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* entranceArea2 = nullptr;

	bool IsUsed = false;

	AventEntrance();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	virtual void Interact_Implementation(class AActor* pActor) override;
	virtual void StopInteract_Implementation(class AActor* pActor) override;
};
