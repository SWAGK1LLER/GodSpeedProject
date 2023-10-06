#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Weapon.h"
#include "Decoy.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UDecoy : public UActorComponent, public IWeapon
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADecoyActor> DecoyActorClass;

	class ABase3C* owner = nullptr;
	class AGamePlayerController* pcCache = nullptr;

	UDecoy();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SpawnDecoy();

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlToggleVisibility(bool visible);
	virtual void MUlToggleVisibility_Implementation(bool visible) {};

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlFire();
	virtual void MUlFire_Implementation();

	void UpdateUI_Implementation() override;
};
