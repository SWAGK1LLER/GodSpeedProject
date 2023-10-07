#pragma once

#include "CoreMinimal.h"
#include "StunWeapon.h"
#include "Baton.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTRANSACK_API UBaton : public UStunWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float coolDown = 2;
	float CoolDownCurrentTime = 0;

	bool isReloading = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int maxAmmo = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ammo = maxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunDuration = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UShapeComponent* HitArea = nullptr;

	TArray<class ABase3C*> possibleHittedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	TArray<TSubclassOf<AActor>> EnemyHittable;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* particleEffect = nullptr;

	UBaton(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void MUlFire_Implementation() override;

	UFUNCTION()
	void OnHitTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void CheckPossibleHit();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MUlPlayAttack();
	void MUlPlayAttack_Implementation();

	UFUNCTION(BlueprintCallable)
	void deactivateTrigger();

	void MUlToggleVisibility_Implementation(bool visible) override;

	bool CheckHittableActor(AActor* pActorToCheck);

	void HitEntity(class AGamePlayerController* PlayerController, AActor* pActorToHit);

	void UpdateUI_Implementation() override;
};
