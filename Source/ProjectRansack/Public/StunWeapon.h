#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "StunWeapon.generated.h"

UCLASS()
class PROJECTRANSACK_API UStunWeapon : public UStaticMeshComponent, public IWeapon
{
	GENERATED_BODY()
public:
	UStunWeapon();

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlToggleVisibility(bool visible);
	virtual void MUlToggleVisibility_Implementation(bool visible) {};

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlFire();
	virtual void MUlFire_Implementation() {};

	void Tick_Implementation(float delta) override {};
	void UpdateUI_Implementation() override {};
};
