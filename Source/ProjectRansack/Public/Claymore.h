#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Claymore.generated.h"

UCLASS()
class PROJECTRANSACK_API AClaymore : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mesh = nullptr;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = SensorGadget, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* trigger = nullptr;

	class ABase3C* owner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunDuration = 5;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* particleEffect = nullptr;

	AClaymore();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHitTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void CheckHit();
};
