#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Weapon.generated.h"

UINTERFACE(BlueprintType)
class UWeapon : public UInterface
{
	GENERATED_BODY()
};

class PROJECTRANSACK_API IWeapon
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlToggleVisibility(bool visible);
	//virtual void MUlToggleVisibility_Implementation(bool visible) {};

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlFire();
	//virtual void MUlFire_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Tick(float delta);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUI();
};