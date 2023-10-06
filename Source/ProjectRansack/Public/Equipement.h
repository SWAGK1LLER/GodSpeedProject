#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Weapon.h>
#include "Grenade.h"
#include "Equipement.generated.h"

UENUM()
enum GrenadeType
{
	None	UMETA(DisplayName = "None"),
	Smoke	UMETA(DisplayName = "Smoke Grenade"),
	Stun	UMETA(DisplayName = "Stun Grenade")
};

UENUM()
enum EquipementPossibility
{
	SmokeGrenade		UMETA(DisplayName = "Smoke Grenade"),
	StunGrenade		UMETA(DisplayName = "Stun Grenade"),
	HoloDecoy	UMETA(DisplayName = "DecoyGadget"),
	InvisibleCloak	UMETA(DisplayName = "Cloak")
};

USTRUCT(BlueprintType)
struct FGrenadeBlueprint
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGrenade> type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* uiImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ammo = 3;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UInvisibleCloak* invisibleCloak;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EquipementPossibility>> utilityBelt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<GrenadeType>, FGrenadeBlueprint> AllGrenade;

	IWeapon* equippedWeapon = nullptr;
	
	class ABase3C* playerCache = nullptr;
	class AGamePlayerController* pcCache = nullptr;

	UEquipement();

	void FinishAttachement(class USceneComponent* root);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(const TScriptInterface<IWeapon>& nextWeapon);

	void UpdateUI();

	void Fire();

	void BindBeltKey(class UEnhancedInputComponent* input, struct FBase3CTable* table);

	void EquipeBelt1();
	void EquipeBelt2();
	void EquipeBelt3();

	const TScriptInterface<IWeapon> GetWeaponFromEnum(EquipementPossibility weapon);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void SetGrenadeType(GrenadeType grenadeType);
	void SetGrenadeType_Implementation(GrenadeType grenadeType);
};
