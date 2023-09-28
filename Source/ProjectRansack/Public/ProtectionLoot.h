#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThiefInteractibleActor.h"
#include "ProtectionLootUI.h"
#include "ProtectionLoot.generated.h"

UCLASS()
class PROJECTRANSACK_API AProtectionLoot : public AActor, public IThiefInteractibleActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class AItem*> itemToActivate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UProtectionLootUI> WidgetClass;
	UProtectionLootUI* Widget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* Trigger = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToHackThief = 5;

	float currentTime = 0;
	bool currentlyInteracting = false;

	class AActor* acteurUsingThis = nullptr;

	AProtectionLoot();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateProgressHack(float DeltaTime);

	void Interact_Implementation(class AActor* pActor) override;

	void StopInteract_Implementation(class AActor* pActor) override;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void DisableSystem();
	void DisableSystem_Implementation();

	virtual void PlayAnimation();
};
