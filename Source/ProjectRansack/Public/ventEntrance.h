// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	AventEntrance();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UShapeComponent* ventArea = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UShapeComponent* entranceArea = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UShapeComponent* entranceArea2 = nullptr;

	bool IsUsed = false;


	UFUNCTION()
		void TriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	virtual void Interact_Implementation(class AActor* pActor) override;
	virtual void StopInteract_Implementation(class AActor* pActor) override;
};
