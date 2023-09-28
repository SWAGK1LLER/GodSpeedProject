#pragma once

#include "CoreMinimal.h"
#include "StunWeapon.h"
#include "StunStick.generated.h"

UCLASS()
class PROJECTRANSACK_API UStunStick : public UStunWeapon
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* HitArea = nullptr;

	TArray<class ABase3C*> possibleHittedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isAttacking = false;

	UStunStick(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Fire() override;

	UFUNCTION()
	void OnHitTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHitTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void CheckPossibleHit();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlPlayAttack();
	void MUlPlayAttack_Implementation();

	UFUNCTION(BlueprintCallable)
	void deactivateTrigger();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlToggleVisibility(bool visible);
	void MUlToggleVisibility_Implementation(bool visible);
};
