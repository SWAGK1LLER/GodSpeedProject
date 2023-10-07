#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Weapon.h"
#include "InvisibleCloak.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTRANSACK_API UInvisibleCloak : public UActorComponent, public IWeapon
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float duration = 3;

	bool abilityUsed = false;

	UInvisibleCloak();

	virtual void BeginPlay() override;

	void MUlFire_Implementation() override;

	void MUlToggleVisibility_Implementation(bool visible) override;

	void UpdateUI_Implementation() override;
};
