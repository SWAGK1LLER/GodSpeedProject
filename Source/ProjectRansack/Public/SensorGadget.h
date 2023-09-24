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
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* sensorGadgetMesh1 = nullptr;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* sensorGadgetMesh2 = nullptr;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionMesh = nullptr;

	class AOfficer* placedActor = nullptr;

	TArray<class AThief*> spottedPlayer;

	bool pinged = false;

	float revealTime = 0.0f;

	ASensorGadget();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetRevealTime(float pRevealTime);

	void CalculateMiddleMesh();

	void PingPlayer(AActor* pPlayerToPing);

	UFUNCTION(NetMulticast, Reliable)
	void MULSetOfficer(class AOfficer* pOwner);
	void MULSetOfficer_Implementation(class AOfficer* pOwner);
};
