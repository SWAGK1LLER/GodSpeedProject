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
	class AGamePlayerController* controller = nullptr;
	class ABase3C* pawn = nullptr;
	bool isActive = false;

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlToggleVisibility(bool visible);

	UFUNCTION(NetMulticast, Reliable, NotBlueprintable)
	virtual void MUlFire();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Tick(float delta);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUI();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayerPossess();
	void PlayerPossess_Implementation() {};
};