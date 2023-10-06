#pragma once

#include "CoreMinimal.h"
#include "StunWeapon.h"
#include "Gun.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTRANSACK_API UGun : public UStunWeapon
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
	float Reach = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunDuration = 5;

	bool isActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	TArray<TSubclassOf<AActor>> EnemyHittable;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* particleEffect = nullptr;

	UGun(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void MUlFire_Implementation() override;

	AActor* HitScan(FVector& hitLocation);

	bool CheckHittableActor(AActor* pActorToCheck);

	void HitEntity(class AGamePlayerController* PlayerController, AActor* pActorToHit);

	virtual void Reload();

	bool isFull() { return ammo == maxAmmo; };

	void MUlToggleVisibility_Implementation(bool visible) override;

	void UpdateUI_Implementation() override;
};
