// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Officer.h"
#include "SensorGadget.generated.h"


UCLASS()
class PROJECTRANSACK_API ASensorGadget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASensorGadget();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;
	

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* sensorGadgetMesh1 = nullptr;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* sensorGadgetMesh2 = nullptr;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionMesh = nullptr;

	UFUNCTION(Server, Reliable)
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetRevealTime(float pRevealTime);

	void CalculateMiddleMesh();

	void SetOfficerOwner(AActor* pOwner);

	UFUNCTION(Server, Reliable)
		void Ser_SetOfficer(AActor* pOwner);

	UFUNCTION(Server, Reliable)
		void Ser_PingPlayer(AActor* pPlayerToPing);

	UFUNCTION(Server, Reliable)
		void Ser_UnPingPlayer(AActor* pPlayerToPing);

	void UnPingPlayer();

	UPROPERTY(Replicated, VisibleAnywhere, blueprintReadWrite)
		AActor* placedActor = nullptr;

	AActor* pingedActor = nullptr;

	bool pinged = false;

	float revealTime = 0.0f;

	FTimerHandle RevealTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
