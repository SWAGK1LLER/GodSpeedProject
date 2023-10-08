#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon.h"
#include "Particles/ParticleSystem.h"
#include "EMP.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UEMP : public UActorComponent, public IWeapon
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunDuration = 5;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* particleEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float cooldown = 1;
	float timer = 0;

	UEMP();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void MUlFire_Implementation() override;

	void MUlToggleVisibility_Implementation(bool visible) override;

	void UpdateUI_Implementation() override;

	void CheckHit();
};
