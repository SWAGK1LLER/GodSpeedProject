#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon.h"
#include "QuietShoes.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UQuietShoes : public UActorComponent, public IWeapon
{
	GENERATED_BODY()

public:	
	UQuietShoes();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlToggleVisibility(bool visible);
	virtual void MUlToggleVisibility_Implementation(bool visible);

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlFire();
	virtual void MUlFire_Implementation() {};

	void UpdateUI_Implementation() override;

	void PlayerPossess_Implementation();
};
