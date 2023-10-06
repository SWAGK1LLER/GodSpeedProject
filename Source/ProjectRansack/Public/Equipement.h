#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Weapon.h>
#include "Equipement.generated.h"

UENUM()
enum EquipementPossibility
{
	SmokeGrenade		UMETA(DisplayName = "Grenade"),
	HoloDecoy	UMETA(DisplayName = "DecoyGadget")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRANSACK_API UEquipement : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGun* StunWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBaton* StunStick = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USensorGadgetOfficerComponent* sensorGadgetOfficer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UGrenadeTrajectory* GrenateTrajectory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UDecoy* decoyGadget;

	TArray<EquipementPossibility> utilityBelt;

	IWeapon* equippedWeapon = nullptr;

	UEquipement();

	void FinishAttachement(class USceneComponent* root);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(const TScriptInterface<IWeapon>& nextWeapon);

	void Fire();
};
