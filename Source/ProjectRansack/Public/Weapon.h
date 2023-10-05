#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Weapon.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UWeapon : public UStaticMeshComponent
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	TArray<TSubclassOf<AActor>> EnemyHittable;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* particleEffect = nullptr;

	UWeapon(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Fire() {};

	virtual void Reload();

	bool isFull() { return ammo == maxAmmo; };
};
